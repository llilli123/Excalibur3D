# Registro de rutas y cambios — M04

## Archivos creados

| Ruta | Propósito |
|---|---|
| `src/modulos/m04_entorno/Entorno.cpp` | Implementación completa de la escenografía M04. |
| `src/modulos/m04_entorno/README.md` | Reglas de propiedad, espacio y futura integración de M04. |
| `docs/Documentacion M4/BITACORA_M04.md` | Bitácora de investigación y pruebas del módulo. |
| `docs/Documentacion M4/CAMBIOS_M04.md` | Este registro de rutas y cambios. |
| `docs/Documentacion M4/ELEMENTOS_M04.md` | Inventario técnico de las figuras de la escena. |

## Archivos modificados

| Ruta | Cambio realizado |
|---|---|
| `CMakeLists.txt` | La fuente de entorno pasó de `src/objetos/Entorno.cpp` a `src/modulos/m04_entorno/Entorno.cpp`. |
| `src/main.cpp` | Modo temporal de prueba: solo llama a `excalibur::objetos::dibujarEntorno()` y nombra la ventana como prueba M04. |
| `src/camara/Camara.cpp` | Se alejaron las tres vistas para encuadrar la escenografía. Es un ajuste temporal de previsualización que debe coordinarse con M08. |

## Archivo trasladado

| Ruta anterior | Ruta actual | Motivo |
|---|---|---|
| `src/objetos/Entorno.cpp` | `src/modulos/m04_entorno/Entorno.cpp` | Separar los archivos de M04 de los objetos compartidos y reducir solapamientos con otros integrantes. |

## Archivos que permanecen sin cambios

- `include/excalibur/Objetos.h`: la firma pública `dibujarEntorno()` se conserva para no romper la integración.
- `src/objetos/Piedra.cpp`: pertenece a M03; M04 solo reserva la zona central donde irá esa piedra.

## Fuera del repositorio

`C:\vcpkg` contiene FreeGLUT para compilar en esta computadora. No se sube al repositorio ni debe agregarse a Git.

## Ajustes para la integración (2026-11-13)
- Se restauró `src/main.cpp` y `src/camara/Camara.cpp` desde `development` para no sobrescribir M08/M10.
- Se devolvió la geometría a `src/objetos/Entorno.cpp` y se restauró `CMakeLists.txt`.
- Se añadió texturizado opcional por M05, con UV repetidas 4 veces y escala escénica 12x12.
