#pragma once
#include <vector>

namespace excalibur::graficos {
    struct Vec3 { float x{}, y{}, z{}; };
    struct Vec2 { float u{}, v{}; };
    struct Malla {
        std::vector<Vec3> posiciones;
        std::vector<Vec3> normales;
        std::vector<Vec2> uv;
        std::vector<unsigned> indices; // triangulos: 3 indices por cara
    };

    // M05. Inicialmente no se cargan texturas. Completar con el cargador del profesor.
    void inicializarTexturas();
    void liberarTexturas();

    // M06. Utilidades disponibles; integrarlas a las mallas creadas por M01-M04.
    Vec3 normalDeCara(Vec3 a, Vec3 b, Vec3 c);
    std::vector<Vec3> normalesSuaves(const std::vector<Vec3>& posiciones,
                                     const std::vector<unsigned>& indices);
    void configurarCulling();

    // M09. Iluminacion clasica OpenGL, punto de partida.
    void inicializarIluminacion();
    void aplicarIluminacion();
}
