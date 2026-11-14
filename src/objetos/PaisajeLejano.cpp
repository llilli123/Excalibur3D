#include <GL/freeglut.h>
#include <cmath>
#include "excalibur/PaisajeLejano.h"
#if __has_include("excalibur/Texturas.h")
#include "excalibur/Texturas.h"
#define EXCALIBUR_PAISAJE_TEXTURAS 1
#endif

// Paisaje lejano: horizonte, terreno que continua mas alla del suelo de M04,
// cadena de montanas low-poly y pinos/rocas en su base.
// Unidades: las del entorno de M04 antes de su glScalef (suelo de -11.5 a 11.5).
// Las tres camaras estan en el cuadrante +X/+Z, por eso las montanas se
// colocan hacia -X/-Z: quedan detras de la piedra y nunca delante de ella.
namespace excalibur::paisaje {
namespace {
constexpr float pi = 3.14159265358979323846f;
constexpr float mitadSuelo = 11.5f;          // borde del suelo texturizado de M04
// Escala grande: tras el glScalef de M04 (16/23) el horizonte queda a ~78
// unidades del centro, dentro del plano lejano de gluPerspective (100).
constexpr float radioFaldon = 108.0f;        // fin del terreno extendido
constexpr float radioHorizonte = 112.0f;     // banda de cielo que cierra la escena
constexpr float alturaHorizonte = 40.0f;

// Color de la bruma: el horizonte, el borde del terreno y la niebla coinciden
// para que no exista un corte visible entre suelo y fondo.
constexpr float bruma[4] = {0.27f, 0.32f, 0.37f, 1.0f};
// Mismo color que glClearColor de main.cpp: la banda se funde con el fondo.
constexpr float cieloAlto[3] = {0.09f, 0.12f, 0.17f};

struct Punto {
    float x, y, z;
};

Punto restar(Punto a, Punto b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
Punto cruz(Punto a, Punto b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}
float producto(Punto a, Punto b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

// Ruido determinista en [-1, 1]: misma forma en cada cuadro y sin rand().
float ruido(int a, int b, int semilla) {
    unsigned h = static_cast<unsigned>(a) * 374761393u + static_cast<unsigned>(b) * 668265263u +
                 static_cast<unsigned>(semilla) * 2246822519u;
    h = (h ^ (h >> 13)) * 1274126177u;
    h ^= h >> 16;
    return static_cast<float>(h & 0xFFFFu) / 32767.5f - 1.0f;
}

float mezclar(float a, float b, float t) { return a + (b - a) * t; }

// ---------------------------------------------------------------------------
// Banda de horizonte: cilindro visto desde dentro, sin luz ni niebla.
// Degradado de bruma (abajo) al color de fondo (arriba).
void dibujarBandaHorizonte() {
    constexpr int segmentos = 48;
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glDepthMask(GL_FALSE); // es fondo: no debe ocultar nada dibujado despues
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segmentos; ++i) {
        const float angulo = 2.0f * pi * static_cast<float>(i) / segmentos;
        const float c = std::cos(angulo);
        const float s = std::sin(angulo);
        glNormal3f(-c, 0.0f, -s); // hacia el centro
        // Arriba -> abajo: caras CCW vistas desde el interior del cilindro.
        glColor3f(cieloAlto[0], cieloAlto[1], cieloAlto[2]);
        glVertex3f(radioHorizonte * c, alturaHorizonte, radioHorizonte * s);
        glColor3f(bruma[0], bruma[1], bruma[2]);
        glVertex3f(radioHorizonte * c, -6.0f, radioHorizonte * s);
    }
    glEnd();
    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
}

// ---------------------------------------------------------------------------
// Faldon de terreno: une el borde cuadrado del suelo con un circulo lejano.
// Usa la misma textura y la misma proyeccion UV que el suelo de M04, de modo
// que la hierba continua sin costura y se oscurece hacia la bruma.
float distanciaAlBordeCuadrado(float c, float s) {
    return mitadSuelo / std::fmax(std::fabs(c), std::fabs(s));
}

void verticeFaldon(float x, float z, float tono) {
    // Proyeccion planar identica a Entorno.cpp: 4 repeticiones en 23 unidades.
    glTexCoord2f((x + mitadSuelo) / (2.0f * mitadSuelo) * 4.0f,
                 (z + mitadSuelo) / (2.0f * mitadSuelo) * 4.0f);
    glColor3f(tono, tono, tono);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(x, -0.004f, z);
}

void dibujarFaldonTerreno() {
    constexpr int segmentos = 64;              // multiplo de 8: incluye las esquinas
    constexpr float desplazamientos[] = {0.0f, 5.0f, 16.0f, 42.0f};
    constexpr float tonos[] = {1.0f, 0.90f, 0.72f, 0.55f};
    constexpr int anillos = sizeof(desplazamientos) / sizeof(desplazamientos[0]);

#ifdef EXCALIBUR_PAISAJE_TEXTURAS
    graficos::activarTextura(graficos::Textura::Suelo);
#else
    glDisable(GL_TEXTURE_2D);
#endif
    for (int a = 0; a <= anillos - 1; ++a) {
        const bool ultimo = (a == anillos - 1);
        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= segmentos; ++i) {
            const float angulo = 2.0f * pi * static_cast<float>(i) / segmentos;
            const float c = std::cos(angulo);
            const float s = std::sin(angulo);
            const float borde = distanciaAlBordeCuadrado(c, s);
            // Exterior -> interior: CCW visto desde +Y (como el agua original).
            const float rExterior = ultimo ? radioFaldon : borde + desplazamientos[a + 1];
            const float rInterior = borde + desplazamientos[a];
            const float tonoExterior = ultimo ? 0.45f : tonos[a + 1];
            verticeFaldon(rExterior * c, rExterior * s, tonoExterior);
            verticeFaldon(rInterior * c, rInterior * s, tonos[a]);
        }
        glEnd();
    }
#ifdef EXCALIBUR_PAISAJE_TEXTURAS
    graficos::desactivarTextura();
#endif
}

// ---------------------------------------------------------------------------
// Montana low-poly: anillos con radio y altura irregulares que convergen en
// una cima desplazada. Normales por cara (aspecto facetado) y color por
// vertice: oscuro y verde en la base, gris roca al medio, claro en la cima.
struct Montana {
    float x, z;        // centro de la base
    float radio;       // radio de la base
    float alto;
    float inclinX, inclinZ; // desplazamiento de la cima para romper la simetria
    int semilla;
};

void colorPorAltura(float t) {
    // Tres tramos: bosque oscuro -> roca -> roca clara/escarcha.
    float r, g, b;
    if (t < 0.45f) {
        const float k = t / 0.45f;
        r = mezclar(0.08f, 0.22f, k); g = mezclar(0.15f, 0.25f, k); b = mezclar(0.10f, 0.20f, k);
    } else if (t < 0.85f) {
        const float k = (t - 0.45f) / 0.40f;
        r = mezclar(0.22f, 0.42f, k); g = mezclar(0.25f, 0.43f, k); b = mezclar(0.20f, 0.44f, k);
    } else {
        const float k = (t - 0.85f) / 0.15f;
        r = mezclar(0.42f, 0.70f, k); g = mezclar(0.43f, 0.72f, k); b = mezclar(0.44f, 0.76f, k);
    }
    glColor3f(r, g, b);
}

void trianguloMontana(Punto a, Punto b, Punto c, const Montana& m) {
    Punto normal = cruz(restar(b, a), restar(c, a));
    const Punto centro{(a.x + b.x + c.x) / 3.0f, (a.y + b.y + c.y) / 3.0f, (a.z + b.z + c.z) / 3.0f};
    const Punto haciaFuera{centro.x - m.x, 0.35f * m.radio, centro.z - m.z};
    if (producto(normal, haciaFuera) < 0.0f) {
        // Garantiza winding CCW visto desde fuera (compatible con GL_CULL_FACE).
        const Punto t = b; b = c; c = t;
        normal = {-normal.x, -normal.y, -normal.z};
    }
    const float largo = std::sqrt(producto(normal, normal));
    if (largo < 1e-6f) return; // descarta triangulos degenerados
    glNormal3f(normal.x / largo, normal.y / largo, normal.z / largo);
    colorPorAltura(a.y / m.alto); glVertex3f(a.x, a.y, a.z);
    colorPorAltura(b.y / m.alto); glVertex3f(b.x, b.y, b.z);
    colorPorAltura(c.y / m.alto); glVertex3f(c.x, c.y, c.z);
}

void dibujarMontana(const Montana& m) {
    constexpr int segmentos = 14;
    constexpr int anillos = 6;
    Punto v[anillos][segmentos];
    for (int k = 0; k < anillos; ++k) {
        const float t = static_cast<float>(k) / anillos;
        // La base se hunde un poco para que no se vea su borde inferior.
        const float y = (k == 0) ? -1.0f : m.alto * t * (1.0f + 0.10f * ruido(k, 99, m.semilla));
        const float cx = m.x + m.inclinX * t;
        const float cz = m.z + m.inclinZ * t;
        for (int i = 0; i < segmentos; ++i) {
            const float angulo = 2.0f * pi * (i + 0.25f * ruido(k, i, m.semilla + 7)) / segmentos;
            const float r = m.radio * std::pow(1.0f - t, 1.05f) * (1.0f + 0.22f * ruido(k, i, m.semilla));
            v[k][i] = {cx + r * std::cos(angulo), y, cz + r * std::sin(angulo)};
        }
    }
    const Punto cima{m.x + m.inclinX, m.alto, m.z + m.inclinZ};

    glBegin(GL_TRIANGLES);
    for (int k = 0; k + 1 < anillos; ++k) {
        for (int i = 0; i < segmentos; ++i) {
            const int j = (i + 1) % segmentos;
            trianguloMontana(v[k][i], v[k][j], v[k + 1][j], m);
            trianguloMontana(v[k][i], v[k + 1][j], v[k + 1][i], m);
        }
    }
    for (int i = 0; i < segmentos; ++i) {
        trianguloMontana(v[anillos - 1][i], v[anillos - 1][(i + 1) % segmentos], cima, m);
    }
    glEnd();
}

// ---------------------------------------------------------------------------
void dibujarPino(float x, float z, float escala, float tono) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // conos/cilindros de FreeGLUT: +Z -> +Y
    glScalef(escala, escala, escala);
    glColor3f(0.20f, 0.13f, 0.08f);
    glutSolidCylinder(0.13, 0.55, 6, 1); // tronco
    glColor3f(0.06f * tono, 0.20f * tono, 0.10f * tono);
    glTranslatef(0.0f, 0.0f, 0.40f);
    glutSolidCone(0.62, 1.20, 7, 2);
    glColor3f(0.08f * tono, 0.24f * tono, 0.12f * tono);
    glTranslatef(0.0f, 0.0f, 0.62f);
    glutSolidCone(0.45, 0.95, 7, 2);
    glPopMatrix();
}

void dibujarRoquita(float x, float z, float escala, float giro) {
    glPushMatrix();
    glTranslatef(x, 0.08f * escala, z);
    glRotatef(giro, 0.0f, 1.0f, 0.0f);
    glScalef(1.3f * escala, 0.55f * escala, 0.95f * escala);
    glColor3f(0.33f, 0.34f, 0.33f);
    glutSolidSphere(0.38, 6, 4); // pocas caras: lectura low-poly
    glPopMatrix();
}
} // namespace

void dibujarPaisajeLejano() {
    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_FOG_BIT | GL_LIGHTING_BIT |
                 GL_TEXTURE_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();

    glDisable(GL_TEXTURE_2D);
    dibujarBandaHorizonte();

    // Niebla suave solo para el paisaje: empieza mas alla del suelo de M04,
    // asi la piedra, la espada y el terreno central no cambian de color.
    // Las distancias son en coordenadas de ojo (ya escaladas).
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogfv(GL_FOG_COLOR, bruma);
    glFogf(GL_FOG_START, 40.0f);
    glFogf(GL_FOG_END, 95.0f);

    dibujarFaldonTerreno();
    glDisable(GL_TEXTURE_2D);

    // Cadena de cuatro montanas grandes y lejanas en el borde posterior.
    // Se dejan huecos detras de la espada para las vistas 1 (~236 grados)
    // y 3 (~222 grados), asi la empunadura se lee contra el cielo.
    const Montana montanas[] = {
        {-62.0f,  36.0f, 17.0f, 20.0f,  1.5f, -1.0f, 11}, // ~150 grados
        {-68.0f, -25.0f, 20.0f, 25.0f, -1.2f,  1.5f, 23}, // ~200 grados
        {-10.0f, -71.0f, 19.0f, 22.0f,  1.8f,  1.0f, 37}, // ~262 grados
        { 36.0f, -62.0f, 16.0f, 18.0f, -1.0f, -1.4f, 51}, // ~300 grados
    };
    for (const Montana& m : montanas) dibujarMontana(m);

    // Pinos y rocas en las faldas, del lado que mira al centro de la escena.
    for (const Montana& m : montanas) {
        const float haciaCentro = std::atan2(-m.z, -m.x);
        for (int k = 0; k < 9; ++k) {
            const float angulo = haciaCentro + 1.5f * ruido(k, 1, m.semilla);
            const float r = m.radio * (1.02f + 0.18f * (0.5f + 0.5f * ruido(k, 2, m.semilla)));
            const float x = m.x + r * std::cos(angulo);
            const float z = m.z + r * std::sin(angulo);
            if (std::fabs(x) < mitadSuelo + 2.0f && std::fabs(z) < mitadSuelo + 2.0f) continue;
            dibujarPino(x, z, 2.6f + 0.9f * ruido(k, 3, m.semilla),
                        0.95f + 0.12f * ruido(k, 4, m.semilla));
        }
        for (int k = 0; k < 3; ++k) {
            const float angulo = haciaCentro + 1.2f * ruido(k, 5, m.semilla);
            const float r = m.radio * 1.08f;
            dibujarRoquita(m.x + r * std::cos(angulo), m.z + r * std::sin(angulo),
                           2.4f + 0.8f * ruido(k, 6, m.semilla), 40.0f * k);
        }
    }

    // Algunos pinos sueltos en el terreno extendido, cerca del suelo central,
    // para que la escala de las montanas se lea por comparacion.
    struct Pino { float x, z, escala, tono; };
    const Pino pinos[] = {
        {-14.5f, -8.0f, 1.3f, 1.00f}, {-16.0f, -6.5f, 1.0f, 0.90f}, {-9.0f, -15.0f, 1.2f, 1.05f},
        {-3.0f, -15.5f, 1.0f, 0.95f}, {6.0f, -14.5f, 1.25f, 1.00f}, {-15.0f, 8.0f, 1.1f, 0.90f},
    };
    for (const Pino& p : pinos) dibujarPino(p.x, p.z, p.escala, p.tono);

    glPopMatrix();
    glPopAttrib();
}
}
