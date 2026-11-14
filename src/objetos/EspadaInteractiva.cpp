#include <GL/freeglut.h>
#include "excalibur/EspadaInteractiva.h"

namespace excalibur::interaccion {
namespace {
    // Cuanto sube la espada (unidades OpenGL). Ajustar si se ve muy poco o demasiado.
    constexpr float ALTURA_EXTRACCION = 1.6f;
    // Duracion de la animacion completa, en segundos.
    constexpr float DURACION_SEGUNDOS = 2.5f;

    float progreso = 0.0f;  // 0 = en la piedra, 1 = suspendida
    int direccion = 0;      // +1 sube, -1 baja, 0 quieta

    // Suavizado smoothstep: arranca y frena despacio, sin saltos.
    float suavizar(float t) {
        return t * t * (3.0f - 2.0f * t);
    }
}

void extraerEspada() { direccion = +1; }
void devolverEspada() { direccion = -1; }

void actualizarEspada(float deltaSegundos) {
    if (direccion == 0) return;
    progreso += static_cast<float>(direccion) * (deltaSegundos / DURACION_SEGUNDOS);
    if (progreso >= 1.0f) { progreso = 1.0f; direccion = 0; }  // queda suspendida
    if (progreso <= 0.0f) { progreso = 0.0f; direccion = 0; }  // vuelve exacta a su sitio
}

void aplicarTransformacionEspada() {
    // Solo se traslada en Y (eje de la espada): sale por el mismo hueco por donde entra,
    // no atraviesa otros objetos y conserva su alineacion. Con progreso = 0 no hay cambio.
    glTranslatef(0.0f, ALTURA_EXTRACCION * suavizar(progreso), 0.0f);
}
}
