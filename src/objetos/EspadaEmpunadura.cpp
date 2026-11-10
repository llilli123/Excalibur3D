#include <GL/freeglut.h>
#include "excalibur/Objetos.h"

namespace excalibur::objetos {
void dibujarEmpunadura() {
    // M02: PREVISUALIZACION. Construir guarda, mango, pomo y detalles.
    glPushMatrix();
    glColor3f(0.88f, 0.67f, 0.25f);
    glTranslatef(0.0f, 2.90f, 0.0f);
    glScalef(0.98f, 0.13f, 0.20f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.35f, 0.19f, 0.10f);
    glTranslatef(0.0f, 3.23f, 0.0f);
    glScalef(0.19f, 0.65f, 0.18f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.88f, 0.67f, 0.25f);
    glTranslatef(0.0f, 3.62f, 0.0f);
    glutSolidSphere(0.18, 20, 14);
    glPopMatrix();
}
}
