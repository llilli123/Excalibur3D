#include <GL/freeglut.h>
#include <algorithm>
#include <cmath>
#include "excalibur/Camara.h"
#include "excalibur/Cuaterniones.h"

namespace excalibur::camara {
namespace {
    using excalibur::matematica::Cuaternion;

    // Tipo local: M08 no amplía el contrato público de Graficos.h solo para
    // almacenar el estado de cámara.
    struct Vec3 {
        float x, y, z;
    };

    // Todas las vistas encuadran el mismo punto, según docs/CONTRATOS.md.
    constexpr Vec3 centroDeInteres{0.0f, 1.5f, 0.0f};
    // Un segundo hace visible el movimiento sin que el control se perciba lento.
    constexpr float duracionTransicion = 1.0f;

    // Posiciones ajustadas al terreno 16x16: mantienen a la espada como foco
    // y dejan leer el monte lejano y los límites del entorno.
    const Vec3 posiciones[] = {
        {5.6f, 4.4f, 8.2f}, // frente
        {7.5f, 4.8f, 0.1f}, // lateral
        {4.6f, 9.4f, 4.1f}  // superior
    };

    Vec3 restar(Vec3 a, Vec3 b) { return {a.x-b.x, a.y-b.y, a.z-b.z}; }
    float producto(Vec3 a, Vec3 b) { return a.x*b.x+a.y*b.y+a.z*b.z; }
    Vec3 cruz(Vec3 a, Vec3 b) {
        return {a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x};
    }
    Vec3 normalizar(Vec3 v) {
        const float longitud = std::sqrt(producto(v, v));
        return longitud > 1e-6f ? Vec3{v.x/longitud, v.y/longitud, v.z/longitud}
                                 : Vec3{0.0f, 0.0f, 1.0f};
    }
    Vec3 interpolar(Vec3 desde, Vec3 hasta, float t) {
        return {desde.x+(hasta.x-desde.x)*t,
                desde.y+(hasta.y-desde.y)*t,
                desde.z+(hasta.z-desde.z)*t};
    }

    // Construye la orientación cámara-a-mundo que mira desde 'posicion' al
    // centro de interés. La cámara de OpenGL mira por su eje local -Z; por
    // ello la tercera base es "atrás" (posición - objetivo).
    //
    // Después se convierte esa base ortonormal a un cuaternión WXYZ, formato
    // acordado por M07/M08 en Cuaterniones.h. Hacer esta conversión una sola
    // vez por vista evita interpolar ángulos de Euler y el gimbal lock.
    Cuaternion orientacionParaMirar(Vec3 posicion) {
        const Vec3 atras = normalizar(restar(posicion, centroDeInteres));
        const Vec3 derecha = normalizar(cruz({0.0f, 1.0f, 0.0f}, atras));
        const Vec3 arriba = cruz(atras, derecha);
        const float traza = derecha.x + arriba.y + atras.z;
        Cuaternion q;
        if (traza > 0.0f) {
            const float s = std::sqrt(traza + 1.0f) * 2.0f;
            q = {0.25f*s, (arriba.z-atras.y)/s, (atras.x-derecha.z)/s,
                 (derecha.y-arriba.x)/s};
        } else if (derecha.x > arriba.y && derecha.x > atras.z) {
            const float s = std::sqrt(1.0f+derecha.x-arriba.y-atras.z)*2.0f;
            q = {(arriba.z-atras.y)/s, 0.25f*s, (arriba.x+derecha.y)/s,
                 (atras.x+derecha.z)/s};
        } else if (arriba.y > atras.z) {
            const float s = std::sqrt(1.0f+arriba.y-derecha.x-atras.z)*2.0f;
            q = {(atras.x-derecha.z)/s, (arriba.x+derecha.y)/s, 0.25f*s,
                 (atras.y+arriba.z)/s};
        } else {
            const float s = std::sqrt(1.0f+atras.z-derecha.x-arriba.y)*2.0f;
            q = {(derecha.y-arriba.x)/s, (atras.x+derecha.z)/s,
                 (atras.y+arriba.z)/s, 0.25f*s};
        }
        return excalibur::matematica::normalizar(q);
    }

    // La matriz MODELVIEW es la inversa de la transformación cámara-a-mundo:
    // primero invierte la orientación q (su traspuesta por ser ortonormal) y
    // después traslada el mundo en la dirección contraria a la cámara.
    // glMultMatrixf recibe la matriz en orden de columnas.
    void aplicarVista(Cuaternion q, Vec3 posicion) {
        q = excalibur::matematica::normalizar(q);
        const float xx=q.x*q.x, yy=q.y*q.y, zz=q.z*q.z;
        const float xy=q.x*q.y, xz=q.x*q.z, yz=q.y*q.z;
        const float wx=q.w*q.x, wy=q.w*q.y, wz=q.w*q.z;
        const GLfloat vista[] = {
            1.0f-2.0f*(yy+zz), 2.0f*(xy-wz),      2.0f*(xz+wy),      0.0f,
            2.0f*(xy+wz),      1.0f-2.0f*(xx+zz), 2.0f*(yz-wx),      0.0f,
            2.0f*(xz-wy),      2.0f*(yz+wx),      1.0f-2.0f*(xx+yy), 0.0f,
            0.0f,               0.0f,               0.0f,               1.0f
        };
        glMultMatrixf(vista);
        glTranslatef(-posicion.x, -posicion.y, -posicion.z);
    }

    // Estado de la transición. Al solicitar otra vista, se captura el estado
    // actual como origen; así, pulsar 1/2/3 durante un movimiento no produce
    // saltos visuales.
    int vistaActual = 0;
    Vec3 posicionInicial = posiciones[0];
    Vec3 posicionFinal = posiciones[0];
    Vec3 posicionActual = posiciones[0];
    Cuaternion orientacionInicial = orientacionParaMirar(posiciones[0]);
    Cuaternion orientacionFinal = orientacionInicial;
    Cuaternion orientacionActual = orientacionInicial;
    float progreso = 1.0f;
}
void seleccionarVista(int indice) {
    // El contrato público usa 1..3; el arreglo interno usa 0..2.
    if (indice < 1 || indice > 3 || vistaActual == indice-1) return;
    vistaActual = indice-1;
    posicionInicial = posicionActual;
    orientacionInicial = orientacionActual;
    posicionFinal = posiciones[vistaActual];
    orientacionFinal = orientacionParaMirar(posicionFinal);
    progreso = 0.0f;
}
void actualizar(float deltaSegundos) {
    // main.cpp entrega delta real y lo limita a 0.05 s. El clamp adicional
    // protege M08 frente a llamadas externas con un delta negativo.
    progreso = std::min(1.0f, progreso + std::max(0.0f, deltaSegundos) / duracionTransicion);
    // La posición se interpola linealmente; la orientación usa SLERP de M07
    // para recorrer el arco corto entre cuaterniones unitarios.
    posicionActual = interpolar(posicionInicial, posicionFinal, progreso);
    orientacionActual = excalibur::matematica::slerp(
        orientacionInicial, orientacionFinal, progreso);
}
void aplicar() {
    // El llamador ya activó y reinició GL_MODELVIEW (ver src/main.cpp).
    aplicarVista(orientacionActual, posicionActual);
}
}
