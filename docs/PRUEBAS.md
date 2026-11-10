# Pruebas de aceptacion (marcar con evidencias)

## Base
- [ ] `cmake -S . -B build && cmake --build build` termina sin errores.
- [ ] El programa abre una ventana, muestra objetos y profundidad correcta.
- [ ] Teclas 1/2/3 cambian de vista; P cambia a alambre; ESC cierra.

## Obligatorios ANTES de entregar
- [ ] M05: foto real en al menos una superficie; UV visibles SIN deformacion evidente.
- [ ] M06: objeto facetado con normal por cara y objeto curvo con normales por vertice.
- [ ] M06: GL_CULL_FACE activado y winding de todas las caras coherente.
- [ ] M07/M08: entre 2 o 3 vistas el movimiento angular es continuo con SLERP real.
- [ ] M07: cuaterniones normalizados, signo del producto escalar y caso de angulo pequeno.
- [ ] No se distorsiona al redimensionar; near del frustum es positivo.
- [ ] Dos integrantes distintos compilan el proyecto desde un clon limpio.
- [ ] La bitacora cabe en una pagina y cita fuentes por autor, titulo y URL.
- [ ] Defensa: cada responsable explica su parte y los tres requisitos.

## Fallas frecuentes
- Ventana negra: revisar matriz, profundidad, luz y que no se haya ocultado por culling.
- Cara invisible: winding incorrecto, cara trasera descartada por GL_CULL_FACE.
- Textura blanca: no se creo/bindeo textura; falta ruta o GL_TEXTURE_2D.
- Textura estirada: UV incorrectas o vertices compartidos con UV distintas.
- Sombreado invertido: producto cruz en orden equivocado; glNormal antes del vertice.
- Transicion brusca: la camara sigue usando vistas instantaneas del scaffold.
- Errores de link: no agregar .cpp a CMakeLists o falta FreeGLUT/GLU.
- Colision en Git: varios editaron `main.cpp` sin avisar.
