#include <GL/freeglut.h>
#include "excalibur/Controles.h"
#include "excalibur/Camara.h"
#include "excalibur/Graficos.h"
#include "excalibur/EspadaInteractiva.h"
#include "excalibur/Audio.h"

namespace excalibur::controles {
void teclado(unsigned char tecla, int /*x*/, int /*y*/) {
    static bool alambre=false;
    switch(tecla) {
        case '1': case '2': case '3':
            excalibur::camara::seleccionarVista(tecla-'0');
            break;
        case 'p': case 'P':
            alambre=!alambre;
            glPolygonMode(GL_FRONT_AND_BACK, alambre ? GL_LINE : GL_FILL);
            break;
#ifdef EXCALIBUR_M09_MATERIALES
        case 'l': case 'L':
            excalibur::graficos::alternarLuzSecundaria();
            break;
#endif
        case 'e': case 'E':
            excalibur::interaccion::extraerEspada();
            break;
        case 'r': case 'R':
            excalibur::interaccion::devolverEspada();
            break;
        case 'm': case 'M':
            excalibur::audio::alternarMusica();
            break;
        case 27: glutLeaveMainLoop(); return; // ESC
        default: break;
    }
    glutPostRedisplay();
    // M10: integrar mas controles; conservar un solo glutKeyboardFunc.
}
}
