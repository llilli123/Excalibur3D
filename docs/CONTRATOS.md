# Contratos entre modulos (leer ANTES de programar)

## Convenciones de espacio
- Eje `Y` vertical, `+Y` hacia arriba. Suelo en `y=0`.
- Espada centrada en `(0, 0, 0)`; hoja base en `y≈1.17` y arriba en `y≈2.83` (placeholder).
- Guarda a `y≈2.90`, mango hasta `y≈3.55`, pomo a `y≈3.62`.
- Piedra centrada en `(0,0.65,0)` y aproximadamente entre `y=0` e `y=1.4`.
- Vistas miran al centro de interes `(0,1.5,0)`; escena total cabe en `[-6,6]` en X/Z.
- Unidades de la escena: convencionales, NO metros exactos.

## Contratos por archivo
`include/excalibur/Objetos.h`: `dibujarHojaEspada`, `dibujarEmpunadura`,
`dibujarPiedra`, `dibujarEntorno`. Ningun modulo de objetos crea ventana ni limpia pantalla.

`include/excalibur/Graficos.h`: Vec3, Vec2, Malla, normales, culling,
texturas y luz. M05 acordara con M01-M04 como pasar ID de textura (nueva interfaz mediante PR).
M06 ofrece `normalDeCara` y `normalesSuaves`, pero los modelos deben usar sus normales.

`include/excalibur/Cuaterniones.h`: `Cuaternion {w,x,y,z}`, `normalizar`,
`slerp(desde,hasta,t)`. Convencion del orden es WXYZ.

`include/excalibur/Camara.h`: seleccionar vista (1..3), actualizar con delta real,
aplicar matrix de vista. M08 solicita a M07 conversiones adicionales si necesita rotacion.

`include/excalibur/Controles.h`: solo M10 registra `glutKeyboardFunc` indirectamente
via main. M08 publica seleccionarVista sin enganchar otro callback.

## Acuerdos de OpenGL
- Pipeline clasico OpenGL (compatible con ejemplos docentes de FreeGLUT).
- `GL_DEPTH_TEST`, `GL_NORMALIZE`, `GL_CULL_FACE` activos; caras frontales CCW.
- Usar `glPushMatrix()/glPopMatrix()` y no dejar estados OpenGL alterados al salir.
- Texturas: establecer UV por vertice y desactivar GL_TEXTURE_2D despues de dibujar.
- Importante: normales deben apuntar hacia fuera; no invertir winding para arreglar culling
  sin revisar el orden de vertices.

## No satisface aun la rubrica
La escena visual de partida NO demuestra: textura de imagen real, mallas propias con
ambos sombreados documentados, ni cambios suaves de camara usando cuaterniones.
Esas son tareas principales, no opcionales.
