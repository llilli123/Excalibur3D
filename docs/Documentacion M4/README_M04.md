# M04 — Entorno medieval

Esta carpeta pertenece exclusivamente al responsable de M04.

- `src/objetos/Entorno.cpp`: suelo, rocas pequeñas y decoración limitada de la escena.
- El punto de integración público se mantiene en `include/excalibur/Objetos.h` como
  `excalibur::objetos::dibujarEntorno()`; no cambies esa firma sin coordinarlo.

## Reglas para modelar

- El suelo está en `y = 0`, con normal hacia `+Y`, caras frontales CCW y UV por vértice.
- Mantén la geometría decorativa fuera de la silueta central.
- Usa mallas simples, cerradas y sin caras superpuestas; M05 aplicará las texturas.
- Cada uso de `glPushMatrix()` debe tener su `glPopMatrix()` correspondiente.

El render principal y la cámara permanecen bajo M10/M08. El entorno ocupa 12x12 unidades en la escena.

