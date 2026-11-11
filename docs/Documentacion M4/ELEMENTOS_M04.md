# Elementos y construcción de la escenografía M04

## Base común: elipsoide UV

La función interna `dibujarElipsoide()` genera una malla cerrada mediante bandas de latitud y cortes de longitud. Cada cuadrilátero recibe UV y una normal analítica orientada hacia fuera. Esta es la base reutilizada por las piezas volumétricas; no son esferas de GLUT sin control de UV.

| Elemento | Construcción | Cantidad / detalle |
|---|---|---|
| Suelo | Un `GL_QUADS` de cuatro vértices. | Normal `+Y`, caras CCW, UV de 0 a 1 y tamaño 23 × 23. |
| Anillo de agua | Un `GL_QUAD_STRIP` circular. | 32 segmentos; radio interior 2.55 y exterior 3.80. Mantiene un islote de tierra central más amplio para la piedra. |
| Canales de agua | Dos `GL_QUAD_STRIP` curvos. | Un canal entra por la izquierda y otro sale por la derecha; cada uno usa cinco pares de puntos de orilla. |
| Rocas pequeñas | Un elipsoide low-poly por roca. | 5 rocas; 8 cortes y 4 bandas, con escala y rotación distintas. |
| Arbustos | Tres elipsoides superpuestos por arbusto. | 4 arbustos; los tres volúmenes crean una silueta vegetal sin hojas o ramas complejas. |
| Madrigueras | Un elipsoide bajo marrón más un elipsoide oscuro y plano. | 3 madrigueras; el primero es el montículo y el segundo representa la entrada. |
| Matas de hierba | Cinco elipsoides verticales y estrechos por mata. | 5 matas (25 hojas volumétricas); se distribuyen de forma irregular para sugerir un claro hacia el centro. |
| Conejos | Elipsoides independientes. | 3 conejos; cada uno tiene cuerpo, cabeza, dos orejas, cola y dos ojos: 7 piezas. Todos miran hacia el islote central. |

## Organización espacial

- El islote central queda libre: allí M03 puede poner la piedra y M01/M02 la espada.
- El anillo de agua está apenas elevado a `y = 0.012` para evitar parpadeo con el suelo (`z-fighting`).
- Rocas, arbustos, madrigueras y hierba se colocan fuera de la silueta central.
- Los colores actuales son temporales mediante `glColor3f`; M05 aplicará materiales y texturas.

## Reglas de integración

1. Mantener la función pública `excalibur::objetos::dibujarEntorno()`.
2. No crear otra ventana, callbacks ni limpiar el framebuffer desde M04.
3. No desactivar `GL_CULL_FACE`; conservar normales hacia fuera y winding CCW.
4. Cada `glPushMatrix()` debe cerrarse con el `glPopMatrix()` correspondiente.
