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

    // M09. Iluminacion clasica OpenGL y materiales (modelo Phong)
    void inicializarIluminacion();
    void aplicarIluminacion();

    enum class Material {
        MetalHoja,
        MetalDorado,
        CueroMango,
        Piedra,
        Entorno,
        Defecto
    };

    void aplicarMaterial(Material mat);
    void materialMetalHoja();
    void materialMetalDorado();
    void materialCuero();
    void materialPiedra();
    void materialEntorno();
    void alternarLuzSecundaria();
    bool estaLuzSecundariaActiva();
}
