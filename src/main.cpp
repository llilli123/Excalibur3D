#include <GL/freeglut.h>
#include <algorithm>
#include <iostream>
#include "excalibur/Objetos.h"
#include "excalibur/Graficos.h"
#include "excalibur/Camara.h"
#include "excalibur/Controles.h"

namespace {
    int ultimoTickMs = 0;

    void dibujarEscena() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        excalibur::camara::aplicar();
        excalibur::graficos::aplicarIluminacion();

        // IMPORTANTE: mantener este orden e implementar cada modulo en su archivo.
        excalibur::objetos::dibujarEntorno();     // M04
        excalibur::objetos::dibujarPiedra();      // M03
        excalibur::objetos::dibujarHojaEspada();  // M01
        excalibur::objetos::dibujarEmpunadura();  // M02

        glutSwapBuffers();
    }

    void alRedimensionar(int ancho, int alto) {
        alto = std::max(alto, 1); // Evita division por cero.
        glViewport(0, 0, ancho, alto);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60.0, static_cast<double>(ancho) / alto, 0.1, 100.0);
        glMatrixMode(GL_MODELVIEW);
    }

    void temporizador(int /*valor*/) {
        const int ahora = glutGet(GLUT_ELAPSED_TIME);
        const float delta = (ultimoTickMs > 0) ? (ahora - ultimoTickMs) / 1000.0f : 0.016f;
        ultimoTickMs = ahora;
        excalibur::camara::actualizar(std::min(delta, 0.05f));
        glutPostRedisplay();
        glutTimerFunc(16, temporizador, 0);
    }

    void inicializarGL() {
        glClearColor(0.09f, 0.12f, 0.17f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_NORMALIZE); // permite usar glScalef y mantener normales unitarias
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glShadeModel(GL_SMOOTH);

        excalibur::graficos::configurarCulling();
        excalibur::graficos::inicializarIluminacion();
        excalibur::graficos::inicializarTexturas();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(1000, 700);
    glutInitWindowPosition(80, 60);
    glutCreateWindow("Excalibur 3D - Base modular CG II");
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    inicializarGL();
    glutDisplayFunc(dibujarEscena);
    glutReshapeFunc(alRedimensionar);
    glutKeyboardFunc(excalibur::controles::teclado);
    glutTimerFunc(16, temporizador, 0);

    std::cout << "Excalibur 3D (base): 1/2/3 vistas, P wireframe, ESC salir.\n"
              << "PENDIENTE: texturas UV reales, normales verificadas y camara SLERP.\n";
    glutMainLoop();
    excalibur::graficos::liberarTexturas();
    return 0;
}
