# Modulo de interaccion y sonido

## Funciones
- **E**: extrae Excalibur de la piedra hasta dejarla suspendida en el aire.
- **R**: devuelve la espada suavemente a su posicion original.
- **M**: activa / desactiva la musica ambiental en bucle.

Archivos: `include/excalibur/EspadaInteractiva.h`, `src/objetos/EspadaInteractiva.cpp`,
`include/excalibur/Audio.h`, `src/audio/Audio.cpp`, `assets/audio/ambiente.wav`.
Integracion minima en `main.cpp`, `Controles.cpp` y `CMakeLists.txt`.

## Transformaciones utilizadas
La hoja (M01) y la empunadura (M02) se dibujan dentro de un mismo `glPushMatrix()/glPopMatrix()`
con un `glTranslatef(0, y, 0)`, por lo que se mueven como un solo objeto sin modificar su geometria.
La traslacion es solo en el eje Y (eje de la espada), asi sale por el mismo hueco de la piedra
sin atravesar otros objetos ni perder su alineacion.

El progreso de la animacion (0 a 1) avanza con el tiempo real (`delta` calculado con
`GLUT_ELAPSED_TIME`), no con los FPS. Se suaviza con smoothstep `t^2 (3 - 2t)`, que evita saltos.
Al terminar queda fijo en 1 (suspendida) o en 0 (posicion original exacta). Si se presiona una
tecla a mitad de la animacion solo cambia la direccion, sin teletransportarse.

## Audio
Se usa WinMM `PlaySoundA` con `SND_FILENAME | SND_ASYNC | SND_LOOP`: reproduce el WAV en segundo
plano, sin bloquear el bucle de GLUT, y lo repite. Al presionar M otra vez (o al cerrar) se llama
`PlaySoundA(nullptr, nullptr, 0)` para detenerlo. En sistemas que no son Windows el codigo compila
igual (sin sonido) gracias a `#ifdef _WIN32`.

**Procedencia del audio:** `assets/audio/ambiente.wav` es musica original creada para este proyecto
mediante sintesis por codigo (melodia de arpa en re dorico con arpegio y fondo suave, ~38 s).
No tiene restricciones de licencia.

## Pruebas realizadas
| # | Prueba | Resultado |
|---|--------|-----------|
| 1 | E extrae completamente la espada y R la devuelve | OK |
| 2 | Presionar E/R repetidamente: cambia de direccion sin saltos | OK |
| 3 | Vistas 1, 2 y 3 funcionan durante la animacion | OK |
| 4 | M reproduce y detiene la musica | OK |
| 5 | P, L y ESC siguen funcionando | OK |
| 6 | Compila y ejecuta sin errores en CLion | OK |
