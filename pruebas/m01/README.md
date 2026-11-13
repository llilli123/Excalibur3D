# Pruebas del módulo M01 (Hoja de Excalibur)

Ejecutan el código real de `src/objetos/EspadaHoja.cpp` con un FreeGLUT **simulado**
(`stub/GL/freeglut.h`) que registra las llamadas a OpenGL en lugar de dibujar. No necesitan
ventana, tarjeta gráfica ni FreeGLUT instalado.

Comprueban: estado OpenGL balanceado, una normal por cara, winding CCW, malla cerrada con las
caras hacia fuera, medidas del contrato, UV en [0, 1] y presencia de los efectos.

## Windows (Developer Command Prompt de Visual Studio)

```bat
cd pruebas\m01
cl /nologo /std:c++17 /W4 /EHsc /I stub /I ..\..\include prueba_m01.cpp ..\..\src\objetos\EspadaHoja.cpp ..\..\src\graficos\Mallas.cpp /Fe:prueba_m01.exe
prueba_m01.exe
```

## Linux / MSYS2 (g++)

```bash
cd pruebas/m01
g++ -std=c++17 -Wall -Wextra -I stub -I ../../include prueba_m01.cpp ../../src/objetos/EspadaHoja.cpp ../../src/graficos/Mallas.cpp -o prueba_m01
./prueba_m01
```

Resultado esperado: `== TODO OK (0 fallos) ==`
