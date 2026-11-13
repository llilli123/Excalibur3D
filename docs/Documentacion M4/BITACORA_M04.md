# Bitácora de investigación — M04 Entorno medieval

**Proyecto:** Excalibur 3D · **Equipo:** G2 · **Módulo:** M04 Entorno · **Fecha:** 2026-11-11

## Fuentes

1. [glFrontFace — docs.GL](https://docs.gl/gl4/glFrontFace), docs.GL; referencia de la API de OpenGL. Se usó para verificar que el proyecto debe conservar el orden CCW en las caras frontales.

## Problema 1: coordenadas UV

No estaba definido cómo dejar modelos listos para texturas sin tener aún las imágenes de M05. Se asignaron `glTexCoord2f` en cada vértice del suelo, los canales, el anillo de agua y los elipsoides. Resultado: las mallas están preparadas para recibir materiales y texturas, aunque por ahora usan color temporal.

## Problema 2: normales, winding y culling

El anillo de agua no se veía porque sus caras estaban orientadas al revés y `GL_CULL_FACE` las descartaba. Se invirtió el orden de sus pares de vértices para que el frente sea CCW visto desde `+Y`. Resultado: el agua se muestra, conserva la normal hacia arriba y no fue necesario desactivar culling.

## Problema 3: integración de cámara

La escena quedaba muy cerrada para comprobar distribución y composición. Se alejaron temporalmente las tres posiciones de `gluLookAt` en la cámara base. El movimiento con cuaterniones/SLERP continúa siendo responsabilidad de M07/M08; M04 no reemplaza esa tarea.

## Código ajeno reutilizado

- No se copió código externo. Las mallas paramétricas y las funciones de escena se escribieron para este proyecto.

## Pruebas realizadas

- `cmake --build build --config Debug`: compilación correcta de `Excalibur3D`.
- Ejecución visual: suelo, anillo de agua, canales, decoración y centro de tierra visibles.
- Culling: el anillo de agua permanece visible con caras frontales CCW.

## Pendientes de integración

- M05 aporta la textura real del suelo; agua y decoración permanecen con color/material.
- Se restauraron las llamadas de M01, M02 y M03 en `main.cpp`; se devolvió la cámara a M08.
- Coordinar con M03 la colocación final de la piedra en el islote central.
