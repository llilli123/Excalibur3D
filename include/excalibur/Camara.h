#pragma once

namespace excalibur::camara {
    void seleccionarVista(int indice); // 1=frente, 2=lateral, 3=superior
    void actualizar(float deltaSegundos);
    void aplicar();  // Llamar con matriz GL_MODELVIEW activa
}
