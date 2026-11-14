#pragma once

// Paisaje lejano y horizonte del entorno (rama feature/ambiente).
// Se llama desde objetos::dibujarEntorno() despues de aplicar la escala del
// entorno; trabaja en las mismas unidades que el terreno de M04 (23x23).
// Guarda y restaura todo el estado OpenGL que modifica.
namespace excalibur::paisaje {
    void dibujarPaisajeLejano();
}
