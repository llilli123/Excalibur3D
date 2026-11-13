#include <GL/freeglut.h>
#include "excalibur/Graficos.h"
#include "excalibur/Objetos.h"

namespace excalibur::objetos {

namespace {

// FreeGLUT orienta sus cilindros sobre +Z. Esta envoltura los coloca sobre +Y,
// que es la convencion de la espada en la escena.
void cilindroVertical(float radio, float alto, int lados = 20) {
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glutSolidCylinder(radio, alto, lados, 2);
}

void aroMango(float y) {
    glPushMatrix();
    glTranslatef(0.0f, y, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glutSolidTorus(0.018f, 0.135f, 8, 20);
    glPopMatrix();
}

} // namespace

void dibujarEmpunadura() {
    // Guarda, mango y pomo finales. El conjunto conserva el estado para no
    // transferir materiales ni colores al siguiente cuadro de la escena.
    glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_ENABLE_BIT);

    // Guarda dorada: barra redondeada y collar que conecta con la hoja.
#ifdef EXCALIBUR_M09_MATERIALES
    graficos::aplicarMaterial(graficos::Material::MetalDorado);
#endif
    glPushMatrix();
    glColor3f(0.84f, 0.60f, 0.16f);
    glTranslatef(0.0f, 2.90f, 0.0f);
    glScalef(0.92f, 0.11f, 0.15f);
    glutSolidSphere(1.0, 24, 12);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 2.84f, 0.0f);
    cilindroVertical(0.17f, 0.16f);
    glPopMatrix();

    // Mango de cuero, con tres aros dorados que rompen la silueta rectangular.
#ifdef EXCALIBUR_M09_MATERIALES
    graficos::aplicarMaterial(graficos::Material::CueroMango);
#endif
    glPushMatrix();
    glColor3f(0.25f, 0.10f, 0.045f);
    glTranslatef(0.0f, 2.96f, 0.0f);
    cilindroVertical(0.115f, 0.58f);
    glPopMatrix();

#ifdef EXCALIBUR_M09_MATERIALES
    graficos::aplicarMaterial(graficos::Material::MetalDorado);
#endif
    glColor3f(0.85f, 0.61f, 0.18f);
    aroMango(3.03f);
    aroMango(3.25f);
    aroMango(3.47f);

    // Pomo pesado, ligeramente aplanado, para que la espada se lea como una
    // pieza terminada desde las tres vistas.
    glPushMatrix();
    glTranslatef(0.0f, 3.62f, 0.0f);
    glScalef(1.0f, 0.88f, 1.0f);
    glutSolidSphere(0.20f, 24, 16);
    glPopMatrix();

    glPopAttrib();
}
}
