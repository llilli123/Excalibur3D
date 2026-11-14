#pragma once

// Anillo de agua, canales y orillas (rama feature/ambiente).
// Se llama desde objetos::dibujarEntorno() dentro de la escala del entorno.
// El agua es semitransparente: se dibuja con GL_BLEND y sin escribir en el
// buffer de profundidad; todo el estado modificado se restaura al salir.
namespace excalibur::agua {
    void dibujarSistemaDeAgua();
}
