// M03 - Piedra (roca irregular)
//
// Idea: crear una esfera con triangulos y "abollarla" para que parezca una roca.
//   PASO 1: calcular los vertices (posicion) y su coordenada de textura (UV).
//   PASO 2: unir los vertices en triangulos (sentido antihorario = cara frontal).
//   PASO 3: calcular las normales suaves con la funcion de M06.
//   Dibujar: recorrer los triangulos y enviarlos a OpenGL.
//
// Contrato: centro en (0, 0.65, 0), base en y = 0 (docs/CONTRATOS.md).
#include <GL/freeglut.h>
#include <cmath>
#include "excalibur/Graficos.h"
#include "excalibur/Objetos.h"
#if __has_include("excalibur/Texturas.h")
#include "excalibur/Texturas.h"
#define EXCALIBUR_M05_DISPONIBLE 1
#endif

namespace excalibur::objetos {
namespace {
using namespace excalibur::graficos;

const float PI = 3.14159265f;
const unsigned ANILLOS = 24;    // cortes de arriba hacia abajo
const unsigned SEGMENTOS = 36;  // cortes alrededor
const unsigned COLUMNAS = SEGMENTOS + 1; // vertices por anillo (el ultimo repite al primero)

// Cuanto se "infla" o "hunde" la roca en cada direccion (suma de ondas).
// Siempre da el mismo resultado para la misma direccion: la roca no cambia.
float bulto(float x, float y, float z) {
    return 0.10f * std::sin(3.1f * x + 1.7f * y) * std::cos(2.3f * z + 0.4f)
         + 0.06f * std::sin(5.3f * y + 2.9f * z + 1.1f)
         + 0.04f * std::cos(7.7f * x - 4.1f * z + 2.0f);
}

Malla crearRoca() {
    Malla roca;

    // PASO 1: vertices y UV.
    for (unsigned i = 0; i <= ANILLOS; i++) {
        float theta = PI * i / ANILLOS;                // 0 = arriba, PI = abajo
        for (unsigned j = 0; j <= SEGMENTOS; j++) {
            float phi = 1.25f * PI + 2 * PI * j / SEGMENTOS; // empieza detras de las camaras

            // Punto de una esfera de radio 1.
            float x = std::sin(theta) * std::cos(phi);
            float y = std::cos(theta);
            float z = std::sin(theta) * std::sin(phi);

            // Abollar, estirar a forma de roca y subir al centro (0, 0.65, 0).
            float r = 1.0f + bulto(x, y, z);
            Vec3 p{1.30f * r * x, 0.65f + 0.76f * r * y, 1.03f * r * z};
            if (p.y < 0.0f) p.y = 0.0f;                  // base plana sobre el suelo
            roca.posiciones.push_back(p);

            // UV: u da 3 vueltas a la imagen alrededor (para no estirarla), v va de abajo a arriba.
            float u = 3.0f * j / SEGMENTOS;
            float v = 1.0f - (float)i / ANILLOS;
            roca.uv.push_back(Vec2{u, v});
        }
    }

    // PASO 2: triangulos. Cada cuadro de la cuadricula se parte en 2 triangulos.
    //   a --- d     a = esquina actual       d = a la derecha
    //   |     |     b = debajo de a          c = debajo de d
    //   b --- c
    for (unsigned i = 0; i < ANILLOS; i++) {
        for (unsigned j = 0; j < SEGMENTOS; j++) {
            unsigned a = i * COLUMNAS + j;
            unsigned b = a + COLUMNAS;
            unsigned c = b + 1;
            unsigned d = a + 1;
            // Orden antihorario visto desde fuera (necesario para GL_CULL_FACE).
            // En los polos se duplican posiciones: omitir el triangulo degenerado.
            // El resto conserva el mismo winding CCW hacia el exterior.
            if (i < ANILLOS - 1) {
                roca.indices.push_back(a); roca.indices.push_back(c); roca.indices.push_back(b);
            }
            if (i > 0) {
                roca.indices.push_back(a); roca.indices.push_back(d); roca.indices.push_back(c);
            }
        }
    }

    // PASO 3: normales suaves (promedio de las caras que tocan cada vertice), de M06.
    roca.normales = normalesSuaves(roca.posiciones, roca.indices);

    // Arreglo de la costura: el primer y el ultimo vertice de cada anillo estan en el
    // mismo lugar; les damos la misma normal para que no se vea una linea en la luz.
    for (unsigned i = 0; i <= ANILLOS; i++) {
        Vec3& n1 = roca.normales[i * COLUMNAS];
        Vec3& n2 = roca.normales[i * COLUMNAS + SEGMENTOS];
        Vec3 promedio{(n1.x + n2.x) / 2, (n1.y + n2.y) / 2, (n1.z + n2.z) / 2};
        n1 = promedio;  // GL_NORMALIZE (activado en main.cpp) la deja de largo 1
        n2 = promedio;
    }

    // Arreglo de los polos: arriba la normal apunta hacia arriba, abajo hacia abajo.
    for (unsigned j = 0; j <= SEGMENTOS; j++) {
        roca.normales[j] = Vec3{0.0f, 1.0f, 0.0f};
        roca.normales[ANILLOS * COLUMNAS + j] = Vec3{0.0f, -1.0f, 0.0f};
    }

    return roca;
}
}

void dibujarPiedra() {
    static Malla roca = crearRoca();   // se crea una sola vez y se reutiliza

#ifdef EXCALIBUR_M09_MATERIALES
    graficos::aplicarMaterial(graficos::Material::Piedra);
#endif
#ifdef EXCALIBUR_M05_DISPONIBLE
    graficos::activarTextura(graficos::Textura::Roca);
    glColor3f(1.0f, 1.0f, 1.0f); // Evita oscurecer la fotografia con GL_MODULATE.
#else
    glColor3f(0.42f, 0.45f, 0.46f); // Apariencia provisional si M05 no esta integrado.
#endif
    glBegin(GL_TRIANGLES);
    for (unsigned k : roca.indices) {
        glNormal3f(roca.normales[k].x, roca.normales[k].y, roca.normales[k].z);
        glTexCoord2f(roca.uv[k].u, roca.uv[k].v);  // normal y UV van ANTES del vertice
        glVertex3f(roca.posiciones[k].x, roca.posiciones[k].y, roca.posiciones[k].z);
    }
    glEnd();
#ifdef EXCALIBUR_M05_DISPONIBLE
    graficos::desactivarTextura();
#endif
}
}