#include "excalibur/Graficos.h"

namespace excalibur::graficos {
void inicializarTexturas() {
    // M05 PENDIENTE:
    // 1. Integrar el cargador de imagen que proporciona el profesor.
    // 2. glGenTextures -> glBindTexture -> glTexParameteri -> glTexImage2D.
    // 3. Proveer IDs/funciones de aplicacion a M01-M04.
    // 4. Definir UV para las mallas (no solo activar GL_TEXTURE_2D).
    // El ZIP NO incluye imagenes ni cargador externo.
}
void liberarTexturas() {
    // M05 PENDIENTE: glDeleteTextures al cerrar, si se crearon.
}
}
