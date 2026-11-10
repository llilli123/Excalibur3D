#include <GL/freeglut.h>
#include "excalibur/Graficos.h"

namespace excalibur::graficos {
void inicializarIluminacion() {
    // M09: PUNTO DE PARTIDA; ajustar materiales, intensidad y posicion.
    const GLfloat ambiente[] = {0.22f, 0.24f, 0.30f, 1.0f};
    const GLfloat difusa[]  = {0.95f, 0.86f, 0.70f, 1.0f};
    const GLfloat especular[]  = {0.45f, 0.45f, 0.45f, 1.0f};
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, difusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, especular);
    const GLfloat brillo[] = {0.32f, 0.32f, 0.32f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, brillo);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);
}
void aplicarIluminacion() {
    // La luz queda fija en coordenadas del mundo si se define tras configurar la camara.
    const GLfloat posicion[] = {-3.5f, 6.0f, 5.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, posicion);
}
}
