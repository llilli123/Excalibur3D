#pragma once

namespace excalibur::interaccion {
    // E: empieza a extraer la espada. R: empieza a devolverla.
    void extraerEspada();
    void devolverEspada();

    // Avanza la animacion segun el tiempo real (delta en segundos), no segun los FPS.
    void actualizarEspada(float deltaSegundos);

    // Aplica la traslacion de la espada. Se llama entre glPushMatrix/glPopMatrix,
    // antes de dibujar la hoja (M01) y la empunadura (M02).
    void aplicarTransformacionEspada();
}
