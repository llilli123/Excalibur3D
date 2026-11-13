# Registro de rutas y cambios — M04

## Archivos creados

| Ruta | Propósito |
|---|---|
| `src/objetos/Entorno.cpp` | Implementación completa de la escenografía M04. |
| `docs/Documentacion M4/README_M04.md` | Reglas de propiedad, espacio y futura integración de M04. |
| `docs/Documentacion M4/BITACORA_M04.md` | Bitácora de investigación y pruebas del módulo. |
| `docs/Documentacion M4/CAMBIOS_M04.md` | Este registro de rutas y cambios. |
| `docs/Documentacion M4/ELEMENTOS_M04.md` | Inventario técnico de las figuras de la escena. |

## Cambios históricos y resolución

Durante el desarrollo M04 trasladó provisionalmente la fuente y modificó `main.cpp`,
`Camara.cpp` y `CMakeLists.txt` para previsualizar el entorno completo.

**Resolución final antes del merge:** la fuente quedó en
`src/objetos/Entorno.cpp`; los tres archivos compartidos fueron restaurados
desde `development`. No se exige a M08 ni a M10 conservar la previsualización.
Se incorporó una escala uniforme de 23 a 12 unidades y la activación de
la textura de suelo solamente cuando el encabezado de M05 está disponible.

## Archivos que permanecen sin cambios

- `include/excalibur/Objetos.h`: la firma pública `dibujarEntorno()` se conserva para no romper la integración.
- `src/objetos/Piedra.cpp`: pertenece a M03; M04 solo reserva la zona central donde irá esa piedra.

## Fuera del repositorio

`C:\vcpkg` contiene FreeGLUT para compilar en esta computadora. No se sube al repositorio ni debe agregarse a Git.

## Ajustes para la integración (2026-11-13)
- Se restauró `src/main.cpp` y `src/camara/Camara.cpp` desde `development` para no sobrescribir M08/M10.
- Se devolvió la geometría a `src/objetos/Entorno.cpp` y se restauró `CMakeLists.txt`.
- Se añadió texturizado opcional por M05, con UV repetidas 4 veces y escala escénica 12x12.
