#include <GL/freeglut.h>
#include "excalibur/Objetos.h"

namespace excalibur::objetos {
void dibujarPiedra() {
    // M03: PREVISUALIZACION. Sustituir por malla de roca irregular;
    // coordinar UV con M05 y normales suaves con M06.
    glPushMatrix();
    glColor3f(0.42f, 0.45f, 0.46f);
    glTranslatef(0.0f, 0.65f, 0.0f);
    glScalef(1.30f, 0.76f, 1.03f);
    glutSolidSphere(1.0, 24, 16);
    glPopMatrix();
}
}
