#include <GL/freeglut.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <initializer_list>
#include <vector>
#include "excalibur/Objetos.h"
#include "excalibur/Graficos.h"

// =============================================================================
// M01 · Hoja de Excalibur
// =============================================================================
//
// 1) GEOMETRIA (requisito R2: objeto FACETADO)
//
//    Seccion transversal de 14 puntos (vista desde arriba):
//
//               P7  P8 P9a  P9b P10 P11            cara frontal (+Z)
//              /    \__/    \__/    \
//     P6 filo <      acanaladura       > P0 filo
//              \    /‾‾\    /‾‾\    /
//               P5  P4 P3b  P3a P2  P1             cara trasera (-Z)
//
//    - Filos (P0, P6) en z = 0; biseles (P0->P1) bajan del plano al filo.
//    - Acanaladura o "fuller" (P2..P4): canal central donde van las runas.
//
//    La seccion se evalua en 18 "anillos" a lo largo de la hoja con la funcion
//    perfil(s), donde s = 0 en la punta y s = 1 en la base:
//    - Ancho: hoja de laurel (se ensancha a media hoja) y punta ojival.
//    - Grosor: "distal taper", la hoja adelgaza hacia la punta como una real.
//    - Acanaladura: nace y muere suavemente (se estrecha y pierde profundidad).
//
//    Cada triangulo lleva UNA normal (producto cruz (b-a)x(c-a), funcion de M06)
//    y no comparte vertices con otras caras: iluminacion plana por cara. Es lo
//    correcto para metal forjado: se ven los cantos de bisel, plano y canal.
//
// 2) COLOR POR VERTICE: filo claro, plano acero, canal azulado. OpenGL interpola
//    el COLOR dentro de la cara, pero la NORMAL sigue siendo una por cara.
//
// 3) EFECTOS (todo el estado OpenGL se guarda con glPushAttrib y se restaura):
//    - Acero pulido: especular alto solo mientras se dibuja la hoja.
//    - Reflejo que recorre la hoja de la guarda a la punta cada pocos segundos.
//    - Luz de contorno ("rim light"): las caras vistas de canto brillan en azul;
//      usa la posicion de la camara sacada de la matriz MODELVIEW.
//    - Runas que se "cargan" en ola desde la guarda hacia la punta.
//    - Aura en 3 capas, chispas en espiral y destello en los filos.
//    - Resplandor donde la hoja entra en la roca (lo tapa la propia roca por
//      profundidad, asi que parece salir de la grieta; no modifica la roca).
//
//    -DEXCALIBUR_HOJA_SIN_EFECTOS  -> solo la malla facetada (para la defensa)
//    -DEXCALIBUR_DEBUG_NORMALES    -> dibuja la normal de cada cara
// =============================================================================

namespace excalibur::objetos {
namespace {
using graficos::Malla;
using graficos::Vec2;
using graficos::Vec3;

constexpr float PI = 3.14159265f;

// ---------------------------------------------------------------------------
// Medidas (docs/CONTRATOS.md). Guarda de M02: cara inferior en y=2.835.
// Piedra de M03: tope en y~1.41. La base (2.85) queda dentro de la guarda y la
// punta (1.17) enterrada en la roca.
// ---------------------------------------------------------------------------
// LARGO DE LA HOJA: lo fija la cara inferior de la guarda de M02. Si el equipo
// sube la guarda (cambio en CONTRATOS.md), basta con cambiar este numero o
// compilar con -DEXCALIBUR_Y_GUARDA_INFERIOR=3.435f: todo lo demas se adapta.
#ifndef EXCALIBUR_Y_GUARDA_INFERIOR
#define EXCALIBUR_Y_GUARDA_INFERIOR 2.835f
#endif
constexpr float Y_BASE = EXCALIBUR_Y_GUARDA_INFERIOR + 0.015f; // 1.5 cm dentro de la guarda
constexpr float Y_PUNTA = 1.17f;
constexpr float LARGO = Y_BASE - Y_PUNTA;
constexpr float S_HOMBRO = 0.28f;      // donde empieza la punta ojival
constexpr float SEMIANCHO_UV = 0.085f; // ancho de referencia para las UV
constexpr float Y_ENTRADA_ROCA = 1.42f;

// Alturas relativas de los anillos (1 = base, 0 = punta). Mas densos en la punta,
// donde la curva cambia mas rapido.
constexpr std::array<float, 18> S_ANILLOS{{1.00f, 0.97f, 0.92f, 0.84f, 0.76f, 0.68f,
                                           0.60f, 0.52f, 0.45f, 0.39f, 0.34f, 0.28f,
                                           0.22f, 0.16f, 0.11f, 0.07f, 0.04f, 0.02f}};

struct Anillo {
    float y;          // altura
    float semiancho;  // del eje al filo
    float grosor;     // del eje a la cara plana
    float bisel;      // x donde termina el bisel
    float canalBorde; // x del borde del canal
    float canalFondo; // x del fondo plano del canal
    float fondo;      // profundidad del canal (0 = sin canal)
};

float suave(float a, float b, float x) { // smoothstep: 0 antes de a, 1 despues de b
    x = std::clamp((x - a) / (b - a), 0.0f, 1.0f);
    return x * x * (3.0f - 2.0f * x);
}

Anillo perfil(float s) {
    Anillo a{};
    a.y = Y_PUNTA + s * LARGO;
    // Ancho: laurel arriba del hombro, ojiva (raiz) hacia la punta.
    // Proporcion de espada larga: la hoja visible mide ~10 veces su ancho.
    if (s >= S_HOMBRO) {
        const float u = (s - S_HOMBRO) / (1.0f - S_HOMBRO);
        a.semiancho = 0.058f + 0.014f * u + 0.009f * std::sin(PI * u);
    } else {
        a.semiancho = 0.058f * std::pow(s / S_HOMBRO, 0.6f);
    }
    // Grosor: constante en la mitad alta, se afina hacia la punta (distal taper).
    if (s >= 0.5f)           a.grosor = 0.026f;
    else if (s >= S_HOMBRO)  a.grosor = 0.021f + 0.005f * (s - S_HOMBRO) / (0.5f - S_HOMBRO);
    else                     a.grosor = 0.021f * std::pow(s / S_HOMBRO, 0.8f);
    // Canal: 0 en la base (tapa convexa) y antes de la punta; 1 en el centro.
    const float k = suave(0.30f, 0.40f, s) * (1.0f - suave(0.93f, 0.985f, s));
    const float w = a.semiancho;
    a.bisel = w - std::min(0.028f, 0.45f * w);
    a.canalBorde = std::min(0.020f * (0.5f + 0.5f * k), 0.75f * a.bisel);
    a.canalFondo = std::min(0.011f * k + 0.003f, 0.6f * a.canalBorde);
    a.fondo = 0.007f * k;
    return a;
}

constexpr int N = 14; // puntos por seccion
enum Zona { FILO, BISEL, PLANO, CANAL };
constexpr std::array<Zona, N> ZONA{{FILO, BISEL, PLANO, CANAL, CANAL, PLANO, BISEL,
                                    FILO, BISEL, PLANO, CANAL, CANAL, PLANO, BISEL}};

// Orden: filo +X, cara trasera (-Z) de +X a -X, filo -X, cara frontal (+Z) de -X a +X.
// Con este orden cada cara lateral queda antihoraria (CCW) vista desde fuera.
std::array<Vec3, N> seccion(const Anillo& a) {
    const float w = a.semiancho, t = a.grosor, b = a.bisel;
    const float f = a.canalBorde, g = a.canalFondo, z = t - a.fondo, y = a.y;
    return {{
        { w, y, 0.0f},
        { b, y, -t}, { f, y, -t}, { g, y, -z}, {-g, y, -z}, {-f, y, -t}, {-b, y, -t},
        {-w, y, 0.0f},
        {-b, y,  t}, {-f, y,  t}, {-g, y,  z}, { g, y,  z}, { f, y,  t}, { b, y,  t},
    }};
}

Vec3 colorDeZona(Zona z) {
    switch (z) {
        case FILO:  return {0.93f, 0.96f, 1.00f};
        case BISEL: return {0.76f, 0.80f, 0.86f};
        case PLANO: return {0.62f, 0.66f, 0.73f};
        default:    return {0.28f, 0.36f, 0.52f};
    }
}

Vec2 uvDe(Vec3 p) { // u de filo a filo, v de la punta (0) a la base (1)
    return {0.5f + p.x / (2.0f * SEMIANCHO_UV), (p.y - Y_PUNTA) / LARGO};
}

struct Vertice { Vec3 p; Vec3 color; };

struct Hoja {
    Malla malla;               // posiciones, normales por cara, UV, indices
    std::vector<Vec3> colores; // color base por vertice
};

// a, b, c en orden CCW vistos desde fuera de la hoja.
void agregarTriangulo(Hoja& h, Vertice a, Vertice b, Vertice c) {
    const Vec3 normal = graficos::normalDeCara(a.p, b.p, c.p);
    for (const Vertice& v : {a, b, c}) {
        h.malla.indices.push_back(static_cast<unsigned>(h.malla.posiciones.size()));
        h.malla.posiciones.push_back(v.p);
        h.malla.normales.push_back(normal);
        h.malla.uv.push_back(uvDe(v.p));
        h.colores.push_back(v.color);
    }
}

Hoja construirHoja() {
    Hoja h;
    std::vector<std::array<Vertice, N>> anillos;
    for (float s : S_ANILLOS) {
        const auto pts = seccion(perfil(s));
        std::array<Vertice, N> anillo{};
        for (int i = 0; i < N; ++i) anillo[i] = {pts[i], colorDeZona(ZONA[i])};
        anillos.push_back(anillo);
    }
    // Cuerpo: entre cada par de anillos, dos triangulos por segmento de la seccion.
    for (std::size_t k = 0; k + 1 < anillos.size(); ++k) {
        const auto& arriba = anillos[k];
        const auto& abajo = anillos[k + 1];
        for (int i = 0; i < N; ++i) {
            const int j = (i + 1) % N;
            agregarTriangulo(h, abajo[i], abajo[j], arriba[j]);
            agregarTriangulo(h, abajo[i], arriba[j], arriba[i]);
        }
    }
    // Punta: el ultimo anillo converge en un solo vertice.
    const Vertice punta{{0.0f, Y_PUNTA, 0.0f}, {0.97f, 0.98f, 1.0f}};
    const auto& ultimo = anillos.back();
    for (int i = 0; i < N; ++i) agregarTriangulo(h, punta, ultimo[(i + 1) % N], ultimo[i]);
    // Tapa superior (dentro de la guarda): seccion sin canal, convexa -> abanico. Normal +Y.
    const auto& base = anillos.front();
    for (int i = 1; i + 1 < N; ++i) agregarTriangulo(h, base[0], base[i], base[i + 1]);
    return h;
}

#ifndef EXCALIBUR_HOJA_SIN_EFECTOS
float tiempoSegundos() { return static_cast<float>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f; }

// Reflejo que baja por la hoja: banda gaussiana en Y, una pasada cada 4 s.
float brilloBarrido(float y, float t) {
    const float fase = std::fmod(t, 4.0f) / 2.2f; // 0..1 dura 2.2 s, luego pausa
    if (fase > 1.0f) return 0.0f;
    const float yBanda = Y_BASE - fase * (LARGO + 0.3f);
    const float d = (y - yBanda) / 0.10f;
    return std::exp(-d * d);
}
#endif

// Pasada solida, iluminada (es la que demuestra el sombreado facetado).
void dibujarMallaIluminada(const Hoja& h, float t) {
    (void)t;
    glBegin(GL_TRIANGLES);
    for (unsigned idx : h.malla.indices) {
        const Vec3& n = h.malla.normales[idx];
        const Vec2& uv = h.malla.uv[idx];
        const Vec3& p = h.malla.posiciones[idx];
        Vec3 c = h.colores[idx];
#ifndef EXCALIBUR_HOJA_SIN_EFECTOS
        const float b = 0.45f * brilloBarrido(p.y, t);
        c = {std::min(1.0f, c.x + b), std::min(1.0f, c.y + b), std::min(1.0f, c.z + b)};
#endif
        glNormal3f(n.x, n.y, n.z);
        glColor3f(c.x, c.y, c.z);
        glTexCoord2f(uv.u, uv.v);
        glVertex3f(p.x, p.y, p.z);
    }
    glEnd();
}

#ifndef EXCALIBUR_HOJA_SIN_EFECTOS
// ---------------------------------------------------------------------------
// Efectos
// ---------------------------------------------------------------------------

// Posicion de la camara en coordenadas de mundo. La hoja no aplica transformacion,
// asi que MODELVIEW = vista = [R | T] y el ojo esta en -R^T * T.
Vec3 posicionCamara() {
    GLfloat m[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, m); // orden por columnas
    const float tx = m[12], ty = m[13], tz = m[14];
    return {-(m[0] * tx + m[1] * ty + m[2] * tz),
            -(m[4] * tx + m[5] * ty + m[6] * tz),
            -(m[8] * tx + m[9] * ty + m[10] * tz)};
}

// Rim light: cuanto mas de canto se ve una cara (normal perpendicular a la vista),
// mas brilla. intensidad = (1 - |n·v|)^3. Una sola intensidad por cara.
void dibujarContorno(const Hoja& h, Vec3 ojo, float pulso) {
    glDepthFunc(GL_LEQUAL); // se dibuja exactamente encima de la pasada solida
    glBegin(GL_TRIANGLES);
    const auto& P = h.malla.posiciones;
    for (std::size_t i = 0; i + 2 < h.malla.indices.size(); i += 3) {
        const Vec3& a = P[h.malla.indices[i]];
        const Vec3& b = P[h.malla.indices[i + 1]];
        const Vec3& c = P[h.malla.indices[i + 2]];
        const Vec3& n = h.malla.normales[h.malla.indices[i]];
        Vec3 v{ojo.x - (a.x + b.x + c.x) / 3.0f, ojo.y - (a.y + b.y + c.y) / 3.0f,
               ojo.z - (a.z + b.z + c.z) / 3.0f};
        const float l = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        const float nv = std::fabs(n.x * v.x + n.y * v.y + n.z * v.z) / l;
        const float rim = std::pow(1.0f - nv, 3.0f) * 0.85f * pulso;
        glColor4f(0.45f, 0.80f, 1.0f, rim);
        glVertex3f(a.x, a.y, a.z);
        glVertex3f(b.x, b.y, b.z);
        glVertex3f(c.x, c.y, c.z);
    }
    glEnd();
    glDepthFunc(GL_LESS);
}

// Aura: la malla inflada en X/Z (la hoja esta centrada en el eje Y, glScalef la
// engorda sin moverla). Tres capas cada vez mas grandes y tenues.
void dibujarAura(const Hoja& h, float pulso) {
    struct Capa { float sx, sz, alfa; };
    for (const Capa& capa : {Capa{1.25f, 1.6f, 0.20f}, Capa{1.6f, 2.6f, 0.11f},
                             Capa{2.1f, 3.8f, 0.05f}}) {
        glPushMatrix();
        glScalef(capa.sx, 1.0f, capa.sz);
        glColor4f(0.35f, 0.75f, 1.0f, capa.alfa * pulso);
        glBegin(GL_TRIANGLES);
        for (unsigned idx : h.malla.indices) {
            const Vec3& p = h.malla.posiciones[idx];
            glVertex3f(p.x, p.y, p.z);
        }
        glEnd();
        glPopMatrix();
    }
}

// Runas originales: trazos en una celda u en [-1,1], v en [0,1].
struct Trazo { float u0, v0, u1, v1; };
const std::vector<std::vector<Trazo>>& runas() {
    static const std::vector<std::vector<Trazo>> r = {
        {{0, 0, 0, 1}, {0, 1, 1, .7f}, {0, .65f, 1, .35f}},
        {{-.6f, 0, -.6f, 1}, {.6f, 0, .6f, 1}, {-.6f, .8f, .6f, .2f}},
        {{0, 0, 0, 1}, {-1, .75f, 0, .45f}, {1, .75f, 0, .45f}},
        {{0, 0, 0, 1}, {0, 1, .9f, .75f}, {.9f, .75f, 0, .5f}, {0, .5f, .9f, 0}},
        {{-.8f, 1, .8f, 0}, {.8f, 1, -.8f, 0}, {-.8f, .5f, .8f, .5f}},
        {{0, 0, 0, 1}, {0, .5f, 1, 1}, {0, .5f, 1, 0}},
        {{-.7f, 0, -.7f, 1}, {-.7f, 1, .7f, .5f}, {.7f, .5f, -.7f, 0}},
    };
    return r;
}

// Runas en el fondo del canal, en las dos caras. Cada runa tiene su brillo:
// una "ola de carga" baja desde la guarda y las va encendiendo una a una.
void trazarRunas(float t, float grosorAlfa) {
    constexpr float alto = 0.06f, separacion = 0.025f, anchoU = 0.008f;
    // Las runas llenan el canal desde la guarda hasta donde el canal sigue hondo
    // (s = 0.42); si la hoja es mas larga, caben mas runas.
    float yTope = Y_BASE - 0.20f;
    const float yLimite = Y_PUNTA + 0.42f * LARGO;
    const int total = static_cast<int>((yTope - yLimite) / (alto + separacion));
    int i = 0;
    glBegin(GL_LINES);
    for (int r = 0; r < total; ++r) {
        const auto& runa = runas()[static_cast<std::size_t>(r) % runas().size()];
        const float yCentro = yTope - alto * 0.5f;
        const Anillo a = perfil((yCentro - Y_PUNTA) / LARGO);
        const float z = a.grosor - a.fondo + 0.0015f; // apenas sobre el fondo del canal
        const float ola = std::fmod(t * 0.45f, 1.0f) * static_cast<float>(total + 2) - static_cast<float>(i);
        const float carga = 0.45f + 0.55f * std::exp(-ola * ola * 0.8f);
        glColor4f(0.55f + 0.3f * carga, 0.9f, 1.0f, grosorAlfa * carga);
        for (const Trazo& s : runa) {
            const float ya = yTope - alto + s.v0 * alto, yb = yTope - alto + s.v1 * alto;
            glVertex3f(s.u0 * anchoU, ya, z);   glVertex3f(s.u1 * anchoU, yb, z);
            glVertex3f(-s.u0 * anchoU, ya, -z); glVertex3f(-s.u1 * anchoU, yb, -z);
        }
        yTope -= alto + separacion;
        ++i;
    }
    glEnd();
}

void dibujarRunas(float t, float pulso) {
    glLineWidth(6.0f);
    trazarRunas(t, 0.30f * pulso); // halo ancho
    glLineWidth(1.6f);
    trazarRunas(t, 1.0f);          // nucleo brillante
}

// Destello que recorre los dos filos.
void dibujarBrilloFilos(float t) {
    const float fase = std::fmod(t * 0.6f, 1.0f);
    const float yDestello = Y_BASE - fase * LARGO;
    glLineWidth(1.5f);
    for (float lado : {1.0f, -1.0f}) {
        glBegin(GL_LINE_STRIP);
        for (float s : S_ANILLOS) {
            const Anillo a = perfil(s);
            const float intensidad = 0.25f + 0.75f * std::exp(-std::fabs(a.y - yDestello) * 6.0f);
            glColor4f(0.85f, 0.95f, 1.0f, intensidad);
            glVertex3f(lado * (a.semiancho + 0.001f), a.y, 0.0f);
        }
        glColor4f(0.85f, 0.95f, 1.0f, 0.6f);
        glVertex3f(0.0f, Y_PUNTA, 0.0f);
        glEnd();
    }
}

// Chispas en espiral alrededor de la hoja; posicion calculada solo con el tiempo.
void dibujarChispas(float t) {
    constexpr int CANTIDAD = 36;
    for (float tam : {8.0f, 2.5f}) {
        glPointSize(tam);
        glBegin(GL_POINTS);
        for (int i = 0; i < CANTIDAD; ++i) {
            const float semilla = static_cast<float>(i);
            const float vida = std::fmod(t * 0.20f + semilla * 0.618f, 1.0f); // 0..1
            // "hash" pseudoaleatorio por chispa: independiente de su altura
            const float h1 = std::fabs(std::fmod(std::sin(semilla * 12.9898f) * 43758.547f, 1.0f));
            const float h2 = std::fabs(std::fmod(std::sin(semilla * 78.233f) * 24634.635f, 1.0f));
            const float radio = 0.04f + 0.07f * h1;
            const float angulo = h2 * 2.0f * PI + vida * 4.0f; // sube girando
            const float x = radio * std::cos(angulo);
            const float z = radio * std::sin(angulo) * 0.6f;
            const float y = Y_ENTRADA_ROCA + vida * (Y_BASE - Y_ENTRADA_ROCA);
            const float alfa = std::sin(vida * PI) * (tam > 3.0f ? 0.22f : 0.9f);
            glColor4f(0.55f, 0.85f, 1.0f, alfa);
            glVertex3f(x, y, z);
        }
        glEnd();
    }
}

// Resplandor en la entrada de la roca: un disco mirando a la camara con brillo
// radial. La roca lo tapa por profundidad, asi que parece salir de la grieta.
void dibujarResplandorEntrada(float pulso) {
    GLfloat m[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    const Vec3 der{m[0], m[4], m[8]};   // eje X de la camara en mundo
    const Vec3 arr{m[1], m[5], m[9]};   // eje Y de la camara en mundo
    constexpr int LADOS = 24;
    const float radio = 0.26f + 0.04f * pulso;
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.6f, 0.9f, 1.0f, 0.38f * pulso);
    glVertex3f(0.0f, Y_ENTRADA_ROCA, 0.0f);
    glColor4f(0.2f, 0.5f, 1.0f, 0.0f);
    for (int i = 0; i <= LADOS; ++i) { // CCW visto desde la camara
        const float a = 2.0f * PI * static_cast<float>(i) / LADOS;
        const float cx = std::cos(a) * radio, cy = std::sin(a) * radio;
        glVertex3f(der.x * cx + arr.x * cy, Y_ENTRADA_ROCA + der.y * cx + arr.y * cy,
                   der.z * cx + arr.z * cy);
    }
    glEnd();
}

void dibujarEfectos(const Hoja& h, float t) {
    const float pulso = 0.75f + 0.25f * std::sin(t * 2.2f); // "respiracion" magica

    glDisable(GL_LIGHTING);            // los efectos emiten luz propia
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // mezcla aditiva: suma brillo
    glDepthMask(GL_FALSE);             // no tapan nada, pero la roca si los tapa
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);

    dibujarContorno(h, posicionCamara(), pulso);
    dibujarRunas(t, pulso);
    dibujarBrilloFilos(t);
    dibujarAura(h, pulso);
    dibujarChispas(t);
    dibujarResplandorEntrada(pulso);
}
#endif

#ifdef EXCALIBUR_DEBUG_NORMALES
void dibujarNormales(const Malla& m) {
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.9f, 0.1f);
    glBegin(GL_LINES);
    for (std::size_t i = 0; i + 2 < m.indices.size(); i += 3) {
        const Vec3& a = m.posiciones[m.indices[i]];
        const Vec3& b = m.posiciones[m.indices[i + 1]];
        const Vec3& c = m.posiciones[m.indices[i + 2]];
        const Vec3& n = m.normales[m.indices[i]];
        const float cx = (a.x + b.x + c.x) / 3.0f;
        const float cy = (a.y + b.y + c.y) / 3.0f;
        const float cz = (a.z + b.z + c.z) / 3.0f;
        constexpr float largo = 0.06f;
        glVertex3f(cx, cy, cz);
        glVertex3f(cx + n.x * largo, cy + n.y * largo, cz + n.z * largo);
    }
    glEnd();
}
#endif
} // namespace

void dibujarHojaEspada() {
    static const Hoja hoja = construirHoja(); // se construye una vez
#ifndef EXCALIBUR_HOJA_SIN_EFECTOS
    const float t = tiempoSegundos();
#else
    const float t = 0.0f;
#endif

    // Guardamos todo el estado que tocamos y lo restauramos al final (CONTRATOS.md).
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT |
                 GL_LINE_BIT | GL_POINT_BIT | GL_CURRENT_BIT);
    glPushMatrix(); // la hoja esta modelada en coordenadas de mundo: sin traslacion

    // Acero pulido: reflejo especular fuerte y concentrado, solo para la hoja.
    const GLfloat especular[] = {0.95f, 0.97f, 1.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, especular);
    glMaterialf(GL_FRONT, GL_SHININESS, 96.0f);
    dibujarMallaIluminada(hoja, t);

#ifndef EXCALIBUR_HOJA_SIN_EFECTOS
    dibujarEfectos(hoja, t);
#endif
#ifdef EXCALIBUR_DEBUG_NORMALES
    dibujarNormales(hoja.malla);
#endif

    glPopMatrix();
    glPopAttrib();
}
} // namespace excalibur::objetos
