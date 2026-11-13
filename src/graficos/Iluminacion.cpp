// Modulo M09 - Iluminacion y Materiales (Modelo Phong)
// Responsable: Integrante 9 - Grupo G5 - Rama: feature/m09-iluminacion
//
// Descripcion tecnica:
// 1. Modelo de Iluminacion Phong / Blinn-Phong:
//    - Luz ambiental global moderada (evita escenas lavadas o totalmente oscuras).
//    - Luz principal (GL_LIGHT0): clave posicional con tono calido medieval y caida suave.
//    - Segunda luz (GL_LIGHT1): relleno frio/mistico en contra-angulo que revela volumenes en sombras.
//    - GL_LIGHT_MODEL_LOCAL_VIEWER activado para calculo exacto del vector de observador.
//    - GL_NORMALIZE activado para mantener normales unitarias tras escalado (glScalef).
// 2. Coordenadas de Mundo:
//    - Las posiciones de las luces se fijan en `aplicarIluminacion()`, inmediatamente
//      despues de que la camara (`excalibur::camara::aplicar()`) configura la matriz
//      de vista en GL_MODELVIEW. De esta forma, OpenGL multiplica la posicion por la matriz
//      de vista, anclando las fuentes luminosas en el espacio de mundo. Al cambiar la camara
//      (vistas 1, 2, 3 o SLERP), las luces permanecen fijas y estables.
// 3. Reflectancia de Materiales (Metal vs Piedra vs Entorno):
//    - Hoja de espada (metal/acero): alta reflectancia especular (0.95) y alto brillo (shininess 96),
//      proporcionando reflejos nitidos y lectura clara de los filos y biseles.
//    - Piedra (roca): reflectancia especular contenida (0.15) y brillo bajo/abierto (shininess 12),
//      modelando relieve y volumen sin producir destellos plasticos artificiales.
//    - Empunadura (oro/cuero) y Entorno (terreno mate).

#include <GL/freeglut.h>
#include "excalibur/Graficos.h"

namespace excalibur::graficos {

namespace {
    bool g_luzSecundariaActiva = true;

    // Parametros de Luz Principal (GL_LIGHT0) - Luz Clave
    const GLfloat g_luz0Ambiente[]  = {0.06f, 0.06f, 0.08f, 1.0f};
    const GLfloat g_luz0Difusa[]    = {0.92f, 0.88f, 0.80f, 1.0f}; // Tono calido natural
    const GLfloat g_luz0Especular[] = {1.00f, 0.98f, 0.92f, 1.0f}; // Destello metalico intenso
    const GLfloat g_luz0Posicion[]  = {-3.5f, 7.0f, 4.5f, 1.0f};   // Coordenadas de mundo

    // Parametros de Segunda Luz (GL_LIGHT1) - Luz de Relleno (Fill Light)
    const GLfloat g_luz1Ambiente[]  = {0.00f, 0.00f, 0.00f, 1.0f};
    const GLfloat g_luz1Difusa[]    = {0.22f, 0.28f, 0.40f, 1.0f}; // Tono azul frio mistico
    const GLfloat g_luz1Especular[] = {0.15f, 0.20f, 0.30f, 1.0f}; // Reflejo sutil de contorno
    const GLfloat g_luz1Posicion[]  = {4.5f, 2.8f, -3.8f, 1.0f};   // Contra-angulo en mundo

    // Luz ambiental global del modelo
    const GLfloat g_modeloAmbiente[] = {0.16f, 0.18f, 0.22f, 1.0f};
}

void inicializarIluminacion() {
    // 1. Habilitar iluminacion y modelo de sombreado suave
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);

    // 2. Garantizar que las normales se re-normalicen tras cualquier glScalef
    glEnable(GL_NORMALIZE);

    // 3. Configuracion global del modelo de luz
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, g_modeloAmbiente);
    // Observador local: calcula vector V exacto desde el vertice a la camara
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    // 4. Luz principal (GL_LIGHT0)
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, g_luz0Ambiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, g_luz0Difusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, g_luz0Especular);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.02f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.001f);

    // 5. Segunda luz opcional de relleno (GL_LIGHT1)
    if (g_luzSecundariaActiva) {
        glEnable(GL_LIGHT1);
        glLightfv(GL_LIGHT1, GL_AMBIENT, g_luz1Ambiente);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, g_luz1Difusa);
        glLightfv(GL_LIGHT1, GL_SPECULAR, g_luz1Especular);
        glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.05f);
    } else {
        glDisable(GL_LIGHT1);
    }

    // 6. Material base inicial por defecto
    aplicarMaterial(Material::Defecto);
}

void aplicarIluminacion() {
    // CONTRATO CRITICO: Esta funcion se invoca desde main.cpp inmediatamente
    // despues de `excalibur::camara::aplicar()`. En este instante exacto, la matriz
    // GL_MODELVIEW contiene exclusivamente la transformacion de la camara (vista).
    // Por lo tanto, al llamar a glLightfv con GL_POSITION, OpenGL premultiplica
    // las coordenadas por la matriz de vista, transformandolas a coordenadas del ojo
    // manteniendo las fuentes de luz estables y fijas en coordenadas del MUNDO.
    glLightfv(GL_LIGHT0, GL_POSITION, g_luz0Posicion);

    if (g_luzSecundariaActiva) {
        glEnable(GL_LIGHT1);
        glLightfv(GL_LIGHT1, GL_POSITION, g_luz1Posicion);
    } else {
        glDisable(GL_LIGHT1);
    }

    // Asegurar que el ciclo de dibujo comience con material neutro
    aplicarMaterial(Material::Defecto);
}

// -------------------------------------------------------------
// Definicion de Materiales y Reflectancia (Phong)
// -------------------------------------------------------------

void materialMetalHoja() {
    // Acero de hoja: alta reflectancia especular y brillo concentrado
    const GLfloat especular[] = {0.92f, 0.95f, 1.00f, 1.0f};
    const GLfloat emision[]   = {0.00f, 0.00f, 0.00f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, especular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 96.0f);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emision);
    glColor3f(0.78f, 0.82f, 0.88f); // Color base acero brillante
}

void materialMetalDorado() {
    // Guarda y pomo: oro pulido medieval
    const GLfloat especular[] = {0.88f, 0.78f, 0.38f, 1.0f};
    const GLfloat emision[]   = {0.00f, 0.00f, 0.00f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, especular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 64.0f);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emision);
    glColor3f(0.88f, 0.68f, 0.22f); // Color oro enriquecido
}

void materialCuero() {
    // Mango de espada: cuero envejecido opaco
    const GLfloat especular[] = {0.08f, 0.08f, 0.08f, 1.0f};
    const GLfloat emision[]   = {0.00f, 0.00f, 0.00f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, especular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 6.0f);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emision);
    glColor3f(0.35f, 0.20f, 0.12f); // Marron cuero oscuro
}

void materialPiedra() {
    // Roca irregular: acabado mate con volumen, brillo no excesivo
    // Un valor especular sutil (0.15) con exponente bajo (12.0)
    // modela los relieves de las normales sin parecer plastico.
    const GLfloat especular[] = {0.15f, 0.15f, 0.16f, 1.0f};
    const GLfloat emision[]   = {0.00f, 0.00f, 0.00f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, especular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 12.0f);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emision);
    glColor3f(0.42f, 0.45f, 0.46f); // Gris piedra natural
}

void materialEntorno() {
    // Plano de suelo: terreno mate no reflectante
    const GLfloat especular[] = {0.04f, 0.04f, 0.04f, 1.0f};
    const GLfloat emision[]   = {0.00f, 0.00f, 0.00f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, especular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 4.0f);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emision);
    glColor3f(0.22f, 0.30f, 0.20f); // Verde musgo / suelo oscuro
}

void aplicarMaterial(Material mat) {
    switch (mat) {
        case Material::MetalHoja:   materialMetalHoja();   break;
        case Material::MetalDorado: materialMetalDorado(); break;
        case Material::CueroMango:  materialCuero();       break;
        case Material::Piedra:      materialPiedra();      break;
        case Material::Entorno:     materialEntorno();     break;
        case Material::Defecto:
        default: {
            const GLfloat especular[] = {0.30f, 0.30f, 0.30f, 1.0f};
            const GLfloat emision[]   = {0.00f, 0.00f, 0.00f, 1.0f};
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, especular);
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emision);
            break;
        }
    }
}

void alternarLuzSecundaria() {
    g_luzSecundariaActiva = !g_luzSecundariaActiva;
    if (g_luzSecundariaActiva) {
        glEnable(GL_LIGHT1);
    } else {
        glDisable(GL_LIGHT1);
    }
}

bool estaLuzSecundariaActiva() {
    return g_luzSecundariaActiva;
}

} // namespace excalibur::graficos
