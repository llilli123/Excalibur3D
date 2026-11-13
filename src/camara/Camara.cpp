#include <GL/freeglut.h>
#include "excalibur/Camara.h"

namespace excalibur::camara {
namespace {
    int vistaActual = 1;
}
void seleccionarVista(int indice) {
    if (indice>=1 && indice<=3) vistaActual=indice;
}
void actualizar(float deltaSegundos) {
    // M08 PENDIENTE: usar excalibur::matematica::slerp() de M07
    // para interpolar la ORIENTACION, y la posicion cuando haga falta.
    (void)deltaSegundos;
}
void aplicar() {
    // VERSION BASE: cambia al instante, por lo tanto NO satisface SLERP.
    // M08 reemplazara esto por orientacion de cuaternion interpolado.
    switch (vistaActual) {
        case 2: gluLookAt(6.0, 3.9, 0.1, 0.0, 1.55, 0.0, 0.0, 1.0, 0.0); break;
        case 3: gluLookAt(3.4, 7.8, 3.0, 0.0, 1.45, 0.0, 0.0, 1.0, 0.0); break;
        default: gluLookAt(4.7, 3.8, 6.5, 0.0, 1.6, 0.0, 0.0, 1.0, 0.0); break;
    }
}
}
