#include <GL/freeglut.h>
#include <cmath>
#include "excalibur/Agua.h"

// Sistema de agua del entorno: anillo alrededor del islote central y dos
// canales que cruzan el terreno. Movido desde Entorno.cpp y ampliado con:
//  - lecho/orilla opaca de tierra oscura bajo el agua,
//  - agua semitransparente con varios tonos de azul por vertice,
//  - brillo animado sutil (solo color: la geometria no se deforma),
//  - piedras pequenas y juncos en las orillas.
// Unidades: las del entorno de M04 antes de su glScalef.
namespace excalibur::agua {
namespace {
constexpr float pi = 3.14159265358979323846f;

constexpr float radioExterior = 3.80f;
constexpr float radioInterior = 2.55f;
constexpr float anchoOrilla = 0.24f;   // franja de tierra visible fuera del agua

// Alturas sobre el suelo (y = 0). El canal va sobre el lecho del anillo para
// cubrir la orilla del anillo en la desembocadura sin z-fighting.
constexpr float alturaLechoAnillo = 0.005f;
constexpr float alturaLechoCanal = 0.008f;
constexpr float alturaAgua = 0.014f;

struct PuntoPlano {
    float x;
    float z;
};

// Paleta por vertice.
struct Color { float r, g, b; };
constexpr Color tierraOrilla{0.19f, 0.14f, 0.09f};
constexpr Color lechoMedio{0.10f, 0.11f, 0.09f};
constexpr Color lechoHondo{0.04f, 0.07f, 0.09f};
constexpr Color aguaSomera{0.22f, 0.52f, 0.62f};
constexpr Color aguaHonda{0.03f, 0.17f, 0.32f};

float tiempoSegundos() {
    return static_cast<float>(glutGet(GLUT_ELAPSED_TIME)) * 0.001f;
}

void colorLecho(int fila, float alfa) {
    // fila 0/4 = orilla de tierra, 1/3 = borde del agua, 2 = centro.
    const Color c = (fila == 0 || fila == 4) ? tierraOrilla : (fila == 2 ? lechoHondo : lechoMedio);
    glColor4f(c.r, c.g, c.b, alfa);
}

// profundidad: 0 = orilla (somera), 1 = centro (honda).
void verticeAgua(float x, float z, float profundidad, float alfa, float t) {
    // Ondas de luz: variacion de brillo de +-6% que se desplaza con el tiempo.
    const float brillo = 1.0f + 0.06f * std::sin(t * 1.4f + x * 1.7f + z * 1.1f)
                              + 0.03f * std::sin(t * 2.3f - x * 0.9f + z * 2.1f);
    const float r = (aguaSomera.r + (aguaHonda.r - aguaSomera.r) * profundidad) * brillo;
    const float g = (aguaSomera.g + (aguaHonda.g - aguaSomera.g) * profundidad) * brillo;
    const float b = (aguaSomera.b + (aguaHonda.b - aguaSomera.b) * profundidad) * brillo;
    // Mas opaca en el centro, mas transparente en la orilla.
    glColor4f(r, g, b, alfa * (0.62f + 0.22f * profundidad));
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(x, alturaAgua, z);
}

// Profundidad del borde exterior del anillo: en las dos desembocaduras
// (angulos 0 y pi) se iguala a la del centro del canal para que no quede una
// franja clara atravesando la union.
float profundidadBordeExterior(float angulo) {
    const float c = std::cos(2.0f * angulo);
    return c > 0.0f ? std::pow(c, 16.0f) : 0.0f;
}

void dibujarLechoAnillo() {
    constexpr int segmentos = 48;
    // Lecho: 5 filas de exterior a interior (tierra, borde, centro, borde, tierra).
    const float radiosLecho[5] = {radioExterior + anchoOrilla, radioExterior,
                                  0.5f * (radioExterior + radioInterior), radioInterior,
                                  radioInterior - anchoOrilla};
    for (int f = 0; f < 4; ++f) {
        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= segmentos; ++i) {
            const float angulo = 2.0f * pi * static_cast<float>(i) / segmentos;
            const float c = std::cos(angulo);
            const float s = std::sin(angulo);
            // Exterior -> interior: caras CCW vistas desde +Y.
            glNormal3f(0.0f, 1.0f, 0.0f);
            colorLecho(f, 1.0f);
            glVertex3f(radiosLecho[f] * c, alturaLechoAnillo, radiosLecho[f] * s);
            glNormal3f(0.0f, 1.0f, 0.0f);
            colorLecho(f + 1, 1.0f);
            glVertex3f(radiosLecho[f + 1] * c, alturaLechoAnillo, radiosLecho[f + 1] * s);
        }
        glEnd();
    }
}

void dibujarAguaAnillo(float t) {
    constexpr int segmentos = 48;
    const float radios[3] = {radioExterior, 0.5f * (radioExterior + radioInterior), radioInterior};
    for (int f = 0; f < 2; ++f) {
        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= segmentos; ++i) {
            const float angulo = 2.0f * pi * static_cast<float>(i) / segmentos;
            const float c = std::cos(angulo);
            const float s = std::sin(angulo);
            const float prof0 = (f == 0) ? profundidadBordeExterior(angulo) : 1.0f;
            const float prof1 = (f == 0) ? 1.0f : 0.0f;
            verticeAgua(radios[f] * c, radios[f] * s, prof0, 1.0f, t);
            verticeAgua(radios[f + 1] * c, radios[f + 1] * s, prof1, 1.0f, t);
        }
        glEnd();
    }
}

// Canal descrito por sus dos orillas, de izquierda a derecha.
// 'desvanecer[i]' atenua el canal al salir del suelo hacia el horizonte.
struct Canal {
    const PuntoPlano* inferior;
    const PuntoPlano* superior;
    const float* desvanecer;
    int puntos;
};

// Interpola 5 filas transversales: 0 = orilla inferior de tierra ... 4 = orilla superior.
PuntoPlano filaCanal(const Canal& canal, int i, int fila, bool conOrilla) {
    const PuntoPlano a = canal.inferior[i];
    const PuntoPlano b = canal.superior[i];
    float dx = b.x - a.x, dz = b.z - a.z;
    const float largo = std::sqrt(dx * dx + dz * dz);
    dx /= largo; dz /= largo;
    const float extra = conOrilla ? anchoOrilla : 0.0f;
    switch (fila) {
        case 0: return {a.x - dx * extra, a.z - dz * extra};
        case 1: return a;
        case 2: return {0.5f * (a.x + b.x), 0.5f * (a.z + b.z)};
        case 3: return b;
        default: return {b.x + dx * extra, b.z + dz * extra};
    }
}

void dibujarLechoCanal(const Canal& canal) {
    for (int f = 0; f < 4; ++f) {
        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i < canal.puntos; ++i) {
            // Fila inferior -> superior, de izquierda a derecha: CCW desde +Y.
            const PuntoPlano p0 = filaCanal(canal, i, f, true);
            const PuntoPlano p1 = filaCanal(canal, i, f + 1, true);
            glNormal3f(0.0f, 1.0f, 0.0f);
            colorLecho(f, canal.desvanecer[i]);
            glVertex3f(p0.x, alturaLechoCanal, p0.z);
            glNormal3f(0.0f, 1.0f, 0.0f);
            colorLecho(f + 1, canal.desvanecer[i]);
            glVertex3f(p1.x, alturaLechoCanal, p1.z);
        }
        glEnd();
    }
}

void dibujarAguaCanal(const Canal& canal, float t) {
    constexpr float profundidadFila[5] = {0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
    for (int f = 1; f < 3; ++f) {
        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i < canal.puntos; ++i) {
            const PuntoPlano p0 = filaCanal(canal, i, f, false);
            const PuntoPlano p1 = filaCanal(canal, i, f + 1, false);
            verticeAgua(p0.x, p0.z, profundidadFila[f], canal.desvanecer[i], t);
            verticeAgua(p1.x, p1.z, profundidadFila[f + 1], canal.desvanecer[i], t);
        }
        glEnd();
    }
}

// --- Orillas -----------------------------------------------------------------
void dibujarPiedrita(float x, float z, float escala, float giro, float tono) {
    glPushMatrix();
    glTranslatef(x, 0.05f * escala, z);
    glRotatef(giro, 0.0f, 1.0f, 0.0f);
    glScalef(1.25f * escala, 0.55f * escala, 0.9f * escala);
    glColor4f(0.36f * tono, 0.36f * tono, 0.34f * tono, 1.0f);
    glutSolidSphere(0.14, 6, 4);
    glPopMatrix();
}

void dibujarJuncos(float x, float z, float escala, float semilla, float t) {
    // Cinco tallos finos (conos cerrados: no desaparecen con el culling).
    const float tallos[5][4] = {
        // dx, dz, altura, inclinacion base
        { 0.00f,  0.00f, 0.95f,  4.0f}, {-0.10f,  0.05f, 0.75f, -9.0f},
        { 0.09f, -0.04f, 0.82f,  8.0f}, {-0.03f, -0.10f, 0.68f, -5.0f},
        { 0.06f,  0.09f, 0.60f, 12.0f},
    };
    for (int k = 0; k < 5; ++k) {
        // Vaiven de +-2.5 grados: sutil y sin coste apreciable.
        const float vaiven = 2.5f * std::sin(t * 1.6f + semilla + k * 0.9f);
        glPushMatrix();
        glTranslatef(x + tallos[k][0] * escala, 0.0f, z + tallos[k][1] * escala);
        glRotatef(tallos[k][3] + vaiven, 0.0f, 0.0f, 1.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // cono de FreeGLUT: +Z -> +Y
        glColor4f(0.30f + 0.03f * k, 0.40f, 0.15f, 1.0f);
        glutSolidCone(0.035 * escala, tallos[k][2] * escala, 5, 1);
        glPopMatrix();
    }
}

void dibujarOrillas(float t) {
    // Piedras en el borde exterior del anillo (lejos de las desembocaduras).
    const float angulosPiedras[] = {0.55f, 0.95f, 1.35f, 2.35f, 2.70f, 3.75f, 4.15f, 4.95f, 5.55f};
    for (int i = 0; i < 9; ++i) {
        const float a = angulosPiedras[i];
        const float r = radioExterior + anchoOrilla * 0.6f + 0.05f * (i % 3);
        dibujarPiedrita(r * std::cos(a), r * std::sin(a), 0.8f + 0.25f * (i % 3), a * 57.0f,
                        0.85f + 0.08f * (i % 2));
    }
    // Piedras en las orillas de los canales.
    dibujarPiedrita(-9.70f, 0.47f, 1.00f, 15.0f, 0.9f);
    dibujarPiedrita(-5.40f, -0.40f, 0.90f, -30.0f, 1.0f);
    dibujarPiedrita(-7.20f, 1.40f, 0.80f, 70.0f, 0.85f);
    dibujarPiedrita(5.40f, -1.40f, 1.05f, 40.0f, 0.95f);
    dibujarPiedrita(8.30f, -1.62f, 0.85f, -10.0f, 0.9f);
    dibujarPiedrita(9.80f, -0.10f, 0.95f, 25.0f, 1.0f);

    // Juncos en algunas partes del anillo y de los canales.
    dibujarJuncos(4.05f * std::cos(1.15f), 4.05f * std::sin(1.15f), 1.0f, 0.0f, t);
    dibujarJuncos(4.05f * std::cos(4.45f), 4.05f * std::sin(4.45f), 0.9f, 1.3f, t);
    dibujarJuncos(4.05f * std::cos(2.55f), 4.05f * std::sin(2.55f), 0.8f, 2.1f, t);
    dibujarJuncos(-8.20f, 1.45f, 1.0f, 3.7f, t);
    dibujarJuncos(-6.00f, -0.30f, 0.85f, 4.4f, t);
    dibujarJuncos(7.10f, 0.20f, 0.95f, 5.2f, t);
    dibujarJuncos(10.40f, -1.98f, 0.80f, 6.0f, t);
}
} // namespace

void dibujarSistemaDeAgua() {
    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_TEXTURE_BIT |
                 GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);

    const float t = tiempoSegundos();

    // Dos canales suaves conectan el anillo con el borde del escenario y se
    // desvanecen fuera del suelo. Su extremo interior termina justo en el
    // borde del anillo (r = 3.80) para no superponer dos capas de agua.
    static const PuntoPlano entradaInferior[] = {
        {-15.40f, 1.55f}, {-13.50f, 1.24f}, {-11.45f, 0.88f}, {-8.80f, 0.42f},
        {-6.40f, 0.02f}, {-4.82f, -0.32f}, {-3.765f, -0.513f}
    };
    static const PuntoPlano entradaSuperior[] = {
        {-15.40f, 2.45f}, {-13.50f, 2.25f}, {-11.45f, 1.90f}, {-8.70f, 1.42f},
        {-6.30f, 0.92f}, {-4.70f, 0.72f}, {-3.736f, 0.695f}
    };
    static const float desvanecerEntrada[] = {0.0f, 0.65f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    static const PuntoPlano salidaInferior[] = {
        {3.731f, -0.724f}, {4.85f, -1.02f}, {6.70f, -1.32f}, {9.00f, -1.48f},
        {11.45f, -1.92f}, {13.50f, -2.22f}, {15.40f, -2.45f}
    };
    static const PuntoPlano salidaSuperior[] = {
        {3.757f, 0.573f}, {4.92f, 0.22f}, {6.76f, -0.12f}, {9.04f, -0.36f},
        {11.45f, -0.28f}, {13.50f, -0.55f}, {15.40f, -1.05f}
    };
    static const float desvanecerSalida[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.65f, 0.0f};
    const Canal entrada{entradaInferior, entradaSuperior, desvanecerEntrada, 7};
    const Canal salida{salidaInferior, salidaSuperior, desvanecerSalida, 7};

    // GL_COLOR_MATERIAL (main.cpp) toma ambiente y difuso del color; el agua
    // anade un brillo especular propio, restaurado por GL_LIGHTING_BIT.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 1) Opacos: lecho y orillas (escriben profundidad normalmente).
    dibujarLechoAnillo();
    dibujarLechoCanal(entrada);
    dibujarLechoCanal(salida);
    dibujarOrillas(t);

    // 2) Agua transparente: sin escribir profundidad.
    const GLfloat especular[4] = {0.55f, 0.60f, 0.65f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, especular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 48.0f);
    glDepthMask(GL_FALSE);
    dibujarAguaAnillo(t);
    dibujarAguaCanal(entrada, t);
    dibujarAguaCanal(salida, t);
    glDepthMask(GL_TRUE);

    glPopMatrix();
    glPopAttrib(); // restaura blend, funcion de blend, depth mask, color, material y textura
}
}
