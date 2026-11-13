# Excalibur 3D - plantilla de trabajo (CG II)

Vitrina 3D inspirada en *la espada en la piedra*, con **C++17, OpenGL clasico y FreeGLUT**.
Preparada para 10 responsables en 5 equipos. **No es la entrega final**: el ZIP contiene
una previsualizacion 3D y puntos de extension `M01` a `M10`. Las texturas UV reales y
la camara con SLERP **estan pendientes**, tal como se indica en el codigo.

> El enunciado del profesor menciona **OpenGL/C** y un esqueleto docente que ya incluye
> un cargador de imagen. Ese esqueleto NO fue adjuntado. Esta plantilla independiente
> utiliza C++17 y debe adaptarse a la base oficial si el profesor la exige. Confirmen
> tambien que acepta 10 integrantes: el documento original dice **parejas**.

## Requisitos
- Compilador C++17, CMake 3.16+, OpenGL + GLU, FreeGLUT (headers y bibliotecas).
- Windows: entorno MSYS2 UCRT64 con `mingw-w64-ucrt-x86_64-gcc`,
  `mingw-w64-ucrt-x86_64-cmake` y `mingw-w64-ucrt-x86_64-freeglut`.
  Ejecute los comandos desde el shell **UCRT64**, no PowerShell con PATH mezclado.
- Linux (Debian/Ubuntu): `sudo apt install build-essential cmake freeglut3-dev libglu1-mesa-dev`

## Compilar
```bash
cmake -S . -B build
cmake --build build
```

En Linux, ejecute `./build/Excalibur3D`.
En Windows, ejecute `build/Excalibur3D.exe` o busque el .exe generado en `build/Debug/`
si utiliza un generador multiconfiguracion (Visual Studio).

## Controles y dependencias
- `1`, `2`, `3`: cambian de vista. Al integrar M07 y M08, la transición usa SLERP.
- `P`: ver en modo alambre para inspeccionar geometria.
- `ESC`: salir.
- `L`: alterna la segunda luz cuando M09 está integrado.

## Escena inicial
Espada simple (cubos), empunadura, roca (esfera deformada), plano de suelo y
una luz clasica. Esta previsualizacion puede visualizarse incluso sin imagenes.
Las implementaciones definitivas se incorporan desde las ramas de los módulos; el repositorio de integración debe demostrar mallas propias, UV y SLERP.

## Distribucion y ramas
| Modulo | Trabajo | Rama |
|---|---|---|
| M01 | Hoja de la espada | `feature/m01-espada-hoja` |
| M02 | Empunadura | `feature/m02-espada-empunadura` |
| M03 | Piedra | `feature/m03-piedra` |
| M04 | Entorno | `feature/m04-entorno` |
| M05 | Texturas UV | `feature/m05-texturas` |
| M06 | Mallas, normales, culling | `feature/m06-normales` |
| M07 | Cuaterniones y SLERP | `feature/m07-cuaterniones` |
| M08 | Camara y transiciones | `feature/m08-camara` |
| M09 | Iluminacion, materiales | `feature/m09-iluminacion` |
| M10 | Controles, integracion | `feature/m10-integracion` |

Lea `docs/CONTRATOS.md`, `docs/PLAN_DE_TRABAJO.md`, `docs/PRUEBAS.md`.
Los nombres exactos de estas ramas estan tambien en el PDF.

## Estrategia Git de este repositorio
- `master`: rama predeterminada, no utilizarla para recibir funcionalidades sin revisión.
- `development`: rama de integración, destino de los Pull Requests aprobados.
- `feature/mXX-*`: rama individual de cada responsable.
- M02 y M06 se completarán manualmente por el equipo; su versión publicada aún puede coincidir con la base.

## Reglas de colaboracion
1. Cada responsable modifica principalmente sus archivos, sin editar `main.cpp` por su cuenta.
2. PRs pequenos, probados; merge a `development` tras revisión; conservar `master` estable.
3. Cambiar firmas de `include/excalibur/*.h` requiere acuerdo previo.
4. No subir la carpeta `build/` ni DLL/binarios ni material descargado con licencia dudosa.
5. Documentar fuentes y fragmentos ajenos. Cada quien debe explicar su codigo en la defensa.
6. Integrar desde el dia 2, no esperar al final.
