#pragma once

namespace excalibur::objetos {
    // Contrato compartido: modelar en unidades OpenGL con eje Y vertical.
    // La base de la piedra esta aproximadamente en y=0 y la espada esta centrada en x=z=0.
    void dibujarHojaEspada();       // M01
    void dibujarEmpunadura();       // M02
    void dibujarPiedra();           // M03
    void dibujarEntorno();          // M04
}
