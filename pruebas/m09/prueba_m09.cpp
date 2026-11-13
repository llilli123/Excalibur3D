// Pruebas de aceptacion de M09 sobre el codigo REAL de src/graficos/Iluminacion.cpp
// Utiliza el stub simulado de FreeGLUT (stub/GL/freeglut.h) que verifica parametros
// del modelo de iluminacion Phong, reflectancia de materiales y coordenadas de mundo.

#include <GL/freeglut.h>
#include <cstdio>
#include <cmath>
#include "excalibur/Graficos.h"

static int fallos = 0;
#define CHECK(cond, msg) do { \
    if (cond) { \
        std::printf("  [OK]    %s\n", msg); \
    } else { \
        std::printf("  [FALLA] %s\n", msg); \
        ++fallos; \
    } \
} while (0)

int main() {
    std::printf("== M09 Iluminacion y Materiales: Pruebas de Aceptacion ==\n\n");

    // -------------------------------------------------------------
    // PRUEBA 1: Inicializacion del Pipeline de Iluminacion
    // -------------------------------------------------------------
    std::printf("1. Configuracion del Pipeline de Iluminacion OpenGL (Phong):\n");
    excalibur::graficos::inicializarIluminacion();
    auto& r = glReg();

    CHECK(r.lightingHabilitado, "GL_LIGHTING habilitado");
    CHECK(r.normalizeHabilitado, "GL_NORMALIZE habilitado (evita distorsion al escalar mallas con glScalef)");
    CHECK(r.light0Habilitado, "Luz principal GL_LIGHT0 habilitada");
    CHECK(r.localViewer == GL_TRUE, "GL_LIGHT_MODEL_LOCAL_VIEWER activo (reflejos especulares exactos y curvos)");

    const float sumaAmbiente = r.globalAmbient[0] + r.globalAmbient[1] + r.globalAmbient[2];
    std::printf("   info: Ambiente global RGB = (%.2f, %.2f, %.2f), suma = %.2f\n",
                r.globalAmbient[0], r.globalAmbient[1], r.globalAmbient[2], sumaAmbiente);
    CHECK(sumaAmbiente >= 0.30f && sumaAmbiente <= 0.90f,
          "Luz ambiental global balanceada (previene escena oscura o sobreexpuesta/lavada)");

    // -------------------------------------------------------------
    // PRUEBA 2: Parametros de la Luz Principal (GL_LIGHT0)
    // -------------------------------------------------------------
    std::printf("\n2. Luz Clave Principal (GL_LIGHT0):\n");
    CHECK(r.luz0.difusa[0] >= 0.85f && r.luz0.difusa[1] >= 0.80f,
          "Difusa con tono calido medieval de alta potencia");
    CHECK(r.luz0.especular[0] >= 0.95f && r.luz0.especular[1] >= 0.95f,
          "Especular pura y potente para destellos en metales");
    CHECK(r.luz0.atenuacionConst == 1.0f && r.luz0.atenuacionLin > 0.0f,
          "Atenuacion configurada (caida suave con la distancia)");

    // -------------------------------------------------------------
    // PRUEBA 3: Segunda Luz Opcional de Relleno (GL_LIGHT1)
    // -------------------------------------------------------------
    std::printf("\n3. Segunda Luz de Relleno (GL_LIGHT1 - Opcional implementada):\n");
    CHECK(r.light1Habilitado && r.luz1.habilitada,
          "Segunda luz GL_LIGHT1 activa por defecto");
    CHECK(r.luz1.difusa[2] > r.luz1.difusa[0],
          "Luz de relleno con tono azul frio/mistico (revela volumen en zonas en sombra)");
    CHECK(r.luz1.ambiente[0] == 0.0f && r.luz1.ambiente[1] == 0.0f,
          "Luz de relleno no suma ambiente extra para no lavar la escena");

    // -------------------------------------------------------------
    // PRUEBA 4: Coordenadas de Mundo y Estabilidad de Luz
    // -------------------------------------------------------------
    std::printf("\n4. Coordenadas de Mundo y Estabilidad con la Camara:\n");
    excalibur::graficos::aplicarIluminacion();
    CHECK(r.luz0.conteoSetPosicion >= 1, "Posicion de LIGHT0 aplicada tras camara");
    CHECK(r.luz0.posicion[3] == 1.0f, "LIGHT0 es luz posicional (w=1.0) en coordenadas de mundo");
    CHECK(r.luz0.posicion[1] > 2.0f, "LIGHT0 posicionada en altura sobre la escena (y > 2.0)");
    CHECK(r.luz1.posicion[3] == 1.0f, "LIGHT1 es posicional en contra-angulo de mundo");

    // -------------------------------------------------------------
    // PRUEBA 5: Reflectancia de Metal (Hoja de Espada)
    // -------------------------------------------------------------
    std::printf("\n5. Reflectancia de Metal de la Hoja (Acero):\n");
    excalibur::graficos::materialMetalHoja();
    CHECK(r.materialActual.especular[0] >= 0.90f && r.materialActual.especular[1] >= 0.90f,
          "Hoja: Alta reflectancia especular metalica (Ks >= 0.90)");
    CHECK(r.materialActual.shininess >= 90.0f,
          "Hoja: Exponente de brillo alto (Shininess >= 90 para filo y biseles nitidos)");
    CHECK(r.materialActual.color[0] > 0.70f && r.materialActual.color[2] > 0.80f,
          "Hoja: Tono base acero pulido claro y legible");

    // -------------------------------------------------------------
    // PRUEBA 6: Reflectancia de Piedra (Roca Irregular)
    // -------------------------------------------------------------
    std::printf("\n6. Reflectancia de Piedra / Roca:\n");
    excalibur::graficos::materialPiedra();
    CHECK(r.materialActual.especular[0] <= 0.20f && r.materialActual.especular[1] <= 0.20f,
          "Piedra: Reflectancia especular moderada/baja (Ks <= 0.20, evita brillo plastico)");
    CHECK(r.materialActual.shininess <= 16.0f && r.materialActual.shininess >= 8.0f,
          "Piedra: Brillo amplio y difuso (Shininess 8..16) que define volumen y relieve");
    CHECK(r.materialActual.color[0] >= 0.35f && r.materialActual.color[0] <= 0.55f,
          "Piedra: Color gris petreo natural");

    // -------------------------------------------------------------
    // PRUEBA 7: Materiales de Oro, Cuero y Terreno
    // -------------------------------------------------------------
    std::printf("\n7. Otros Materiales de la Escena:\n");
    excalibur::graficos::materialMetalDorado();
    CHECK(r.materialActual.especular[0] > r.materialActual.especular[2] && r.materialActual.shininess >= 60.0f,
          "Oro: Destello dorado calido con brillo concentrado");

    excalibur::graficos::materialCuero();
    CHECK(r.materialActual.especular[0] <= 0.10f && r.materialActual.shininess <= 10.0f,
          "Cuero: Superficie opaca y mate para el mango");

    excalibur::graficos::materialEntorno();
    CHECK(r.materialActual.especular[0] <= 0.05f,
          "Entorno: Suelo mate sin reflejo especular molesto");

    // -------------------------------------------------------------
    // PRUEBA 8: Control Dinamico de Luz Secundaria
    // -------------------------------------------------------------
    std::printf("\n8. Control Dinamico de Luz Secundaria:\n");
    CHECK(excalibur::graficos::estaLuzSecundariaActiva() == true, "Luz secundaria inicialmente activa");
    excalibur::graficos::alternarLuzSecundaria();
    CHECK(excalibur::graficos::estaLuzSecundariaActiva() == false, "Alternar desactiva luz secundaria");
    CHECK(!r.light1Habilitado, "GL_LIGHT1 se deshabilito en OpenGL");
    excalibur::graficos::alternarLuzSecundaria();
    CHECK(excalibur::graficos::estaLuzSecundariaActiva() == true, "Alternar reactiva luz secundaria");

    // -------------------------------------------------------------
    // RESUMEN
    // -------------------------------------------------------------
    std::printf("\n== %s (%d fallos) ==\n", fallos ? "HAY FALLOS" : "TODO OK", fallos);
    return fallos ? 1 : 0;
}
