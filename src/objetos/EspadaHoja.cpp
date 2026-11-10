#include <GL/freeglut.h>
#include "excalibur/Objetos.h"

namespace excalibur::objetos {
void dibujarHojaEspada() {
    // M01: PREVISUALIZACION. Reemplazar por hoja con vertice de punta,
    // triangulos propios, winding CCW y normales por cara verificadas.
    glPushMatrix();
    glColor3f(0.75f, 0.79f, 0.84f);
    glTranslatef(0.0f, 2.0f, 0.0f);
    glScalef(0.19f, 1.65f, 0.13f);
    glutSolidCube(1.0);
    glPopMatrix();
}
}
