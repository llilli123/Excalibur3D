# Pruebas del modulo M09 (Iluminacion y Materiales)

Ejecutan las funciones reales de `src/graficos/Iluminacion.cpp` utilizando un FreeGLUT **simulado** (`stub/GL/freeglut.h`) que registra y valida las llamadas a OpenGL.

## Que verifican estas pruebas
1. **Pipeline de Iluminacion**: `GL_LIGHTING`, `GL_NORMALIZE`, `GL_LIGHT0`, `GL_LIGHT1` y `GL_LIGHT_MODEL_LOCAL_VIEWER`.
2. **Balance Visual**: La luz ambiental global evita escenas oscuras o sobreexpuestas/lavadas.
3. **Luz Clave (LIGHT0)**: Espectro calido difuso y destello especular puro con atenuacion realista por distancia.
4. **Luz de Relleno (LIGHT1)**: Relleno frio en contra-angulo que revela volumen en las sombras.
5. **Coordenadas de Mundo**: Posicion 4D ($w=1.0$) fijada tras la camara, garantizando estabilidad ante cambios de vista.
6. **Reflectancia de Metal**: Hoja con alta reflectancia especular ($K_s \ge 0.90$) y brillo concentrado ($Shininess \ge 90$).
7. **Reflectancia de Piedra**: Roca con reflectancia contenida ($K_s \le 0.20$) y brillo no excesivo ($Shininess \le 16$) para modelar volumen sin parecer plastico.
8. **Materiales secundarios**: Oro para guarda/pomo, cuero mate para mango y terreno no reflectante.
9. **Control Dinamico**: Alternancia de la segunda luz en tiempo de ejecucion.

## Compilacion y Ejecucion

### Windows (Developer Command Prompt de Visual Studio)
```cmd
cd pruebas\m09
cl /nologo /std:c++17 /W4 /EHsc /I stub /I ..\..\include prueba_m09.cpp ..\..\src\graficos\Iluminacion.cpp /Fe:prueba_m09.exe
prueba_m09.exe
```

### Linux / MSYS2 (g++)
```bash
cd pruebas/m09
g++ -std=c++17 -Wall -Wextra -I stub -I ../../include prueba_m09.cpp ../../src/graficos/Iluminacion.cpp -o prueba_m09
./prueba_m09
```

Resultado esperado: `== TODO OK (0 fallos) ==`
