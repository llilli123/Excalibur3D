#pragma once
// =============================================================================
//  EXCALIBUR 3D · M05 - Texturas y coordenadas UV
// =============================================================================
#include "excalibur/Graficos.h"

namespace excalibur::graficos {

    enum class Textura { Roca = 0, Suelo, Total };

    // Activa la textura antes de dibujar el objeto
    void activarTextura(Textura t);
    // La apaga despues (para que la espada no la herede)
    void desactivarTextura();

    // UV del suelo: proyeccion planar desde arriba.
    // tamanoLado = ancho del plano (12 en la escena); repeticiones = veces que se repite la foto.
    Vec2 uvPlanarXZ(float x, float z, float tamanoLado, float repeticiones);

    // UV de la roca: proyeccion esferica. (dx,dy,dz) = vertice - centro de la roca.
    Vec2 uvEsferica(float dx, float dy, float dz);

    // Arregla la costura de la proyeccion esferica (donde u salta de 1 a 0).
    void corregirCostura(Vec2& a, Vec2& b, Vec2& c);
}
