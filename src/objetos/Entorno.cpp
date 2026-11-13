#include <GL/freeglut.h>
#include <cmath>
#include "excalibur/Objetos.h"
#include "excalibur/Graficos.h"
#if __has_include("excalibur/Texturas.h")
#include "excalibur/Texturas.h"
#define EXCALIBUR_M05_DISPONIBLE 1
#endif

namespace excalibur::objetos {
namespace {
constexpr float pi = 3.14159265358979323846f;

struct PuntoPlano {
    float x;
    float z;
};

void verticeElipsoide(float radioX, float radioY, float radioZ,
                      float latitud, float longitud, float u, float v) {
    const float cosLatitud = std::cos(latitud);
    const float x = radioX * cosLatitud * std::cos(longitud);
    const float y = radioY * std::sin(latitud);
    const float z = radioZ * cosLatitud * std::sin(longitud);

    // Normal analitica de un elipsoide; mantiene el sombreado correcto al escalarlo.
    float normalX = x / (radioX * radioX);
    float normalY = y / (radioY * radioY);
    float normalZ = z / (radioZ * radioZ);
    const float longitudNormal = std::sqrt(normalX * normalX + normalY * normalY + normalZ * normalZ);
    normalX /= longitudNormal;
    normalY /= longitudNormal;
    normalZ /= longitudNormal;

    glNormal3f(normalX, normalY, normalZ);
    glTexCoord2f(u, v);
    glVertex3f(x, y, z);
}

// Malla cerrada de baja densidad con UV para que M05 pueda texturizarla después.
void dibujarElipsoide(float radioX, float radioY, float radioZ, int cortes, int bandas) {
    for (int banda = 0; banda < bandas; ++banda) {
        const float v0 = static_cast<float>(banda) / bandas;
        const float v1 = static_cast<float>(banda + 1) / bandas;
        const float latitud0 = -pi * 0.5f + pi * v0;
        const float latitud1 = -pi * 0.5f + pi * v1;

        glBegin(GL_QUADS);
        for (int corte = 0; corte < cortes; ++corte) {
            const float u0 = static_cast<float>(corte) / cortes;
            const float u1 = static_cast<float>(corte + 1) / cortes;
            const float longitud0 = 2.0f * pi * u0;
            const float longitud1 = 2.0f * pi * u1;

            // CCW visto desde fuera: compatible con GL_CULL_FACE.
            verticeElipsoide(radioX, radioY, radioZ, latitud0, longitud0, u0, v0);
            verticeElipsoide(radioX, radioY, radioZ, latitud1, longitud0, u0, v1);
            verticeElipsoide(radioX, radioY, radioZ, latitud1, longitud1, u1, v1);
            verticeElipsoide(radioX, radioY, radioZ, latitud0, longitud1, u1, v0);
        }
        glEnd();
    }
}

void dibujarRoca(float x, float z, float escala, float rotacion) {
    glPushMatrix();
    glTranslatef(x, 0.24f * escala, z);
    glRotatef(rotacion, 0.0f, 1.0f, 0.0f);
    glScalef(1.15f, 0.72f, 0.88f);
    glColor3f(0.34f, 0.35f, 0.31f);
    dibujarElipsoide(0.42f * escala, 0.34f * escala, 0.42f * escala, 8, 4);
    glPopMatrix();
}

void dibujarArbusto(float x, float z, float escala) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);

    // Tres volúmenes separados dan una silueta vegetal sin ramas u hojas complejas.
    glColor3f(0.12f, 0.30f, 0.13f);
    glPushMatrix();
    glTranslatef(-0.16f * escala, 0.34f * escala, 0.02f * escala);
    dibujarElipsoide(0.30f * escala, 0.37f * escala, 0.28f * escala, 8, 5);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.19f * escala, 0.31f * escala, 0.07f * escala);
    dibujarElipsoide(0.28f * escala, 0.34f * escala, 0.27f * escala, 8, 5);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.02f * escala, 0.49f * escala, -0.13f * escala);
    dibujarElipsoide(0.30f * escala, 0.35f * escala, 0.27f * escala, 8, 5);
    glPopMatrix();

    glPopMatrix();
}

void dibujarCanal(const PuntoPlano* orillaInferior, const PuntoPlano* orillaSuperior,
                  int cantidadPuntos) {
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i < cantidadPuntos; ++i) {
        const float u = static_cast<float>(i) / (cantidadPuntos - 1);
        // Inferior y superior, de izquierda a derecha: winding CCW desde arriba.
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(u, 0.0f);
        glVertex3f(orillaInferior[i].x, 0.012f, orillaInferior[i].z);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(u, 1.0f);
        glVertex3f(orillaSuperior[i].x, 0.012f, orillaSuperior[i].z);
    }
    glEnd();
}

void dibujarSistemaDeAgua() {
    constexpr int segmentos = 32;
    constexpr float radioExterior = 3.80f;
    constexpr float radioInterior = 2.55f;

    glColor3f(0.06f, 0.25f, 0.35f);

    // Anillo circular: reserva un islote central para la piedra y la espada.
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segmentos; ++i) {
        const float u = static_cast<float>(i) / segmentos;
        const float angulo = 2.0f * pi * u;
        const float coseno = std::cos(angulo);
        const float seno = std::sin(angulo);

        // Exterior -> interior genera caras CCW vistas desde +Y.
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(u, 0.0f);
        glVertex3f(radioExterior * coseno, 0.012f, radioExterior * seno);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(u, 1.0f);
        glVertex3f(radioInterior * coseno, 0.012f, radioInterior * seno);
    }
    glEnd();

    // Dos flujos suaves conectan el anillo al borde del escenario.
    constexpr PuntoPlano entradaInferior[] = {
        {-11.45f, 0.88f}, {-8.80f, 0.42f}, {-6.40f, 0.02f}, {-4.82f, -0.32f}, {-3.66f, -0.50f}
    };
    constexpr PuntoPlano entradaSuperior[] = {
        {-11.45f, 1.90f}, {-8.70f, 1.42f}, {-6.30f, 0.92f}, {-4.70f, 0.72f}, {-3.50f, 0.65f}
    };
    constexpr PuntoPlano salidaInferior[] = {
        {3.62f, -0.70f}, {4.85f, -1.02f}, {6.70f, -1.32f}, {9.00f, -1.48f}, {11.45f, -1.92f}
    };
    constexpr PuntoPlano salidaSuperior[] = {
        {3.75f, 0.55f}, {4.92f, 0.22f}, {6.76f, -0.12f}, {9.04f, -0.36f}, {11.45f, -0.28f}
    };
    dibujarCanal(entradaInferior, entradaSuperior, 5);
    dibujarCanal(salidaInferior, salidaSuperior, 5);
}

void dibujarMadriguera(float x, float z, float escala, float rotacion) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(rotacion, 0.0f, 1.0f, 0.0f);

    // Pequeño montículo y boca oscura: es legible sin perforar ni deformar el suelo.
    glColor3f(0.27f, 0.20f, 0.12f);
    glPushMatrix();
    glTranslatef(0.0f, 0.11f * escala, 0.0f);
    dibujarElipsoide(0.45f * escala, 0.16f * escala, 0.31f * escala, 10, 4);
    glPopMatrix();
    glColor3f(0.035f, 0.026f, 0.018f);
    glPushMatrix();
    glTranslatef(0.0f, 0.14f * escala, -0.28f * escala);
    dibujarElipsoide(0.20f * escala, 0.13f * escala, 0.026f * escala, 10, 5);
    glPopMatrix();

    glPopMatrix();
}

void dibujarMataDeHierba(float x, float z, float escala, float rotacion) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(rotacion, 0.0f, 1.0f, 0.0f);
    glColor3f(0.18f, 0.38f, 0.15f);

    // Hojas volumétricas simples con UV, en vez de planos que desaparecen por culling.
    const float desplazamientos[][3] = {
        { 0.00f, 0.14f,  0.00f}, {-0.12f, 0.11f,  0.03f},
        { 0.13f, 0.10f, -0.02f}, {-0.03f, 0.10f, -0.11f},
        { 0.06f, 0.09f,  0.11f}
    };
    for (const auto& hoja : desplazamientos) {
        glPushMatrix();
        glTranslatef(hoja[0] * escala, hoja[1] * escala, hoja[2] * escala);
        glRotatef(hoja[0] * 180.0f, 0.0f, 1.0f, 0.0f);
        dibujarElipsoide(0.040f * escala, 0.20f * escala, 0.040f * escala, 6, 4);
        glPopMatrix();
    }
    glPopMatrix();
}

void dibujarConejo(float x, float z) {
    const float direccionHaciaCentro = std::atan2(x, z) * 180.0f / pi;
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(direccionHaciaCentro, 0.0f, 1.0f, 0.0f);

    // El frente local es -Z; tras la rotación el conejo mira hacia (0, 0, 0).
    glColor3f(0.70f, 0.67f, 0.57f);
    glPushMatrix();
    glTranslatef(0.0f, 0.28f, 0.05f);
    dibujarElipsoide(0.25f, 0.28f, 0.38f, 10, 6); // cuerpo
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f, 0.56f, -0.27f);
    dibujarElipsoide(0.20f, 0.20f, 0.19f, 10, 6); // cabeza
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-0.09f, 0.84f, -0.29f);
    glRotatef(-12.0f, 1.0f, 0.0f, 0.0f);
    dibujarElipsoide(0.06f, 0.25f, 0.07f, 8, 5); // oreja izquierda
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.09f, 0.84f, -0.29f);
    glRotatef(-12.0f, 1.0f, 0.0f, 0.0f);
    dibujarElipsoide(0.06f, 0.25f, 0.07f, 8, 5); // oreja derecha
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f, 0.30f, 0.41f);
    dibujarElipsoide(0.11f, 0.11f, 0.11f, 8, 5); // cola
    glPopMatrix();

    // Ojos oscuros pequeños, como volúmenes independientes para material posterior.
    glColor3f(0.05f, 0.04f, 0.03f);
    glPushMatrix();
    glTranslatef(-0.12f, 0.60f, -0.39f);
    dibujarElipsoide(0.028f, 0.028f, 0.022f, 8, 4);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.12f, 0.60f, -0.39f);
    dibujarElipsoide(0.028f, 0.028f, 0.022f, 8, 4);
    glPopMatrix();

    glPopMatrix();
}
} // namespace

void dibujarEntorno() {
    // M04 se modelo en 23x23 unidades; presentar la vitrina en 12x12,
    // sin escalar los modulos de piedra y espada.
    glPushMatrix();
    constexpr float escalaEscena = 12.0f / 23.0f;
    glScalef(escalaEscena, escalaEscena, escalaEscena);
#ifdef EXCALIBUR_M09_MATERIALES
    graficos::aplicarMaterial(graficos::Material::Entorno);
#endif
#ifdef EXCALIBUR_M05_DISPONIBLE
    graficos::activarTextura(graficos::Textura::Suelo);
    glColor3f(1.0f, 1.0f, 1.0f);
#endif
    // Suelo: cuatro vértices, normal +Y, winding CCW y UV completos.
    glColor3f(0.22f, 0.30f, 0.20f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-11.5f, 0.0f, -11.5f);
    glTexCoord2f(0.0f, 4.0f); glVertex3f(-11.5f, 0.0f,  11.5f);
    glTexCoord2f(4.0f, 4.0f); glVertex3f( 11.5f, 0.0f,  11.5f);
    glTexCoord2f(4.0f, 0.0f); glVertex3f( 11.5f, 0.0f, -11.5f);
    glEnd();
#ifdef EXCALIBUR_M05_DISPONIBLE
    graficos::desactivarTextura();
#endif
    dibujarSistemaDeAgua();

    // Todo queda fuera del área central reservada para la piedra y la espada.
    dibujarRoca(-8.4f, -5.5f, 0.95f, 18.0f);
    dibujarRoca(-7.3f, -6.15f, 0.58f, -21.0f);
    dibujarRoca(8.1f, -5.65f, 0.84f, 34.0f);
    dibujarRoca(8.9f, 4.4f, 0.74f, -12.0f);
    dibujarRoca(-8.1f, 5.8f, 0.88f, 27.0f);
    dibujarArbusto(-8.6f, 7.1f, 1.05f);
    dibujarArbusto(8.45f, 6.7f, 0.96f);
    dibujarArbusto(-9.0f, -1.4f, 0.88f);
    dibujarArbusto(7.4f, 1.8f, 0.92f);

    // Tres entradas discretas: el conejo cercano da contexto sin ocupar el foco.
    dibujarMadriguera(-5.85f, -4.35f, 0.94f, -18.0f);
    dibujarMadriguera(-1.75f, -7.45f, 1.12f, 24.0f);
    dibujarMadriguera(4.50f, 6.15f, 0.88f, 152.0f);

    // Grupos irregulares que flanquean un claro y sugieren el acceso al centro.
    dibujarMataDeHierba(-4.80f, -3.80f, 1.04f, -22.0f);
    dibujarMataDeHierba(-3.65f, -2.72f, 0.80f, 13.0f);
    dibujarMataDeHierba(-2.55f, -5.40f, 0.78f, -28.0f);
    dibujarMataDeHierba(2.35f, -5.45f, 0.93f, 31.0f);
    dibujarMataDeHierba(4.15f, -3.25f, 0.74f, -14.0f);
    dibujarConejo(5.40f, -4.70f);
    dibujarConejo(-5.30f, -4.15f);
    dibujarConejo(5.80f, 4.40f);
    glPopMatrix();
}
}
