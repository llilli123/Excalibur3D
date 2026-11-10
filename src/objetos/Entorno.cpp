#include <GL/freeglut.h>
#include "excalibur/Objetos.h"

namespace excalibur::objetos {
void dibujarEntorno() {
    // M04: PREVISUALIZACION. Plano con normal hacia +Y y winding CCW.
    glColor3f(0.22f, 0.30f, 0.20f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-6.0f, 0.0f, -6.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-6.0f, 0.0f,  6.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 6.0f, 0.0f,  6.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 6.0f, 0.0f, -6.0f);
    glEnd();
}
}
