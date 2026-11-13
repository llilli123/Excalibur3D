# Documentación Oficial - Módulo M09: Iluminación y Materiales
**Proyecto:** Excalibur 3D (Computación Gráfica II)  
**Grupo:** G5  
**Responsable:** Integrante 9  
**Rama de trabajo:** `feature/m09-iluminacion`  
**Archivo propio:** `src/graficos/Iluminacion.cpp`  
**Cabecera vinculada:** `include/excalibur/Graficos.h`  

---

## 1. Resumen y Objetivos del Módulo

El propósito del módulo **M09** es diseñar, calibrar e integrar el sistema de **iluminación y reflectancia de materiales** para la vitrina tridimensional de *Excalibur 3D*, basándose en el pipeline de función fija de OpenGL clásico bajo el modelo de iluminación de **Phong / Blinn-Phong**.

### Requerimientos Específicos:
1. **Luz Phong de base:** Configuración de iluminación ambiental, difusa y especular mediante `glLightfv` y `glLightModelfv`.
2. **Reflectancia de metal y piedra:** Diferenciación precisa de coeficientes de material con `glMaterialfv` y `glMaterialf(..., GL_SHININESS)`.
3. **Coordenadas de mundo:** Posicionar las fuentes luminosas en coordenadas absolutas de mundo tras la cámara, garantizando que permanezcan estables al cambiar de vista.
4. **Segunda luz opcional:** Implementación de una luz secundaria de relleno (Fill Light) para acentuar el volumen y el relieve de la escena en zonas de penumbra.
5. **Solución del error de CMake:** Corregir el fallo de configuración en Windows bajo Visual Studio por ausencia de GLUT en el sistema.

### Criterios de Aceptación:
* Hoja de la espada metálica, brillante y con filos/biseles perfectamente legibles.
* Piedra con volumen definido y textura rugosa con brillo contenido (no excesivo ni con apariencia plástica).
* Fuentes de luz estables ante los cambios de cámara (vistas 1, 2, 3 o interpolación SLERP).
* Escena visualmente equilibrada: sin zonas oscuras impenetrables ni sobreexposición/lavado de color.

---

## 2. Corrección del Error de CMake en Windows / Visual Studio

### Causa del Error
El `CMakeLists.txt` base utilizaba la instrucción:
```cmake
find_package(GLUT REQUIRED)
```
En plataformas Linux o entornos MSYS2, FreeGLUT se instala en rutas globales del sistema (`/usr/include` o `/ucrt64/include`). Sin embargo, en Windows bajo Visual Studio:
1. El SDK oficial de Microsoft incluye únicamente `OpenGL32.lib` y `GLU32.lib`, pero **no incluye FreeGLUT**.
2. Al no estar en el PATH ni en las carpetas de Visual Studio, CMake detenía la configuración con:
   ```text
   CMake Error: Could NOT find GLUT (missing: GLUT_glut_LIBRARY)
   ```
3. Adicionalmente, el CMake moderno integrado en Visual Studio (CMake 4.3) bloqueaba configuraciones de proyectos con políticas mínimas de CMake anteriores a 3.5.

### Solución Técnica Implementada
Se modificó [CMakeLists.txt](file:///C:/Users/Junior%20duran/source/repos/Excalibur3D/CMakeLists.txt) incorporando:
1. **Búsqueda condicional:** Se cambió a `find_package(GLUT)`. Si GLUT ya existe en el sistema (compañeros en Linux o MSYS2), se utiliza de inmediato.
2. **Descarga y compilación automática con `FetchContent`:** Si no se encuentra GLUT, CMake descarga automáticamente el repositorio oficial de **FreeGLUT 3.4.0** y lo compila como biblioteca estática (`freeglut_static`).
3. **Enlace estático sin dependencias externas:** Al vincular `freeglut_static` con el alias `GLUT::GLUT`, el código de FreeGLUT se incorpora directamente dentro de `Excalibur3D.exe`, eliminando la necesidad de colocar `freeglut.dll` en carpetas del sistema.
4. **Políticas y advertencias MSVC:**
   - `set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)` para compatibilidad con CMake 4.3+.
   - Flags `/EHsc` (manejo de excepciones C++) y `/wd4505` (supresión de advertencias por funciones inline en cabeceras de FreeGLUT), logrando una compilación limpia con **cero advertencias**.

```cmake
# Fragmento clave en CMakeLists.txt
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)
find_package(OpenGL REQUIRED)
find_package(GLUT)

if(NOT GLUT_FOUND)
    message(STATUS "GLUT no encontrado. Descargando y compilando FreeGLUT automaticamente con FetchContent...")
    include(FetchContent)
    FetchContent_Declare(
        freeglut
        GIT_REPOSITORY https://github.com/freeglut/freeglut.git
        GIT_TAG v3.4.0
    )
    set(FREEGLUT_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
    set(FREEGLUT_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(freeglut)

    if(TARGET freeglut_static AND NOT TARGET GLUT::GLUT)
        add_library(GLUT::GLUT ALIAS freeglut_static)
    endif()
    set(GLUT_INCLUDE_DIRS ${freeglut_SOURCE_DIR}/include)
endif()
```

---

## 3. Fundamentos Teóricos: Modelo de Iluminación Phong

El pipeline clásico de OpenGL implementa el modelo de reflexión de **Phong / Blinn-Phong**. La intensidad total de color en cada vértice resulta de la combinación lineal:

$$I = I_{\text{emisión}} + I_{\text{ambiente\_global}} + \sum_{i=0}^{N-1} \text{Atenuación}_i \cdot \Big[ I_{\text{amb}, i} + I_{\text{dif}, i} + I_{\text{esp}, i} \Big]$$

### A. Componente Ambiental ($I_{\text{amb}}$)
Representa la luz indirecta multidireccional producida por múltiples rebotes en el entorno:
$$I_{\text{amb}} = K_a \cdot L_a$$
* Se calibró una luz ambiente global de `(0.16, 0.18, 0.22, 1.0)`. Su ligero tinte azulado emula una ambientación crepuscular o mística, impidiendo que las zonas ocultas caigan en negro absoluto sin reducir el contraste general de la escena.

### B. Componente Difusa - Reflexión Lambertiana ($I_{\text{dif}}$)
Modela la dispersión uniforme de la luz en superficies rugosas u opacas. Depende del coseno del ángulo de incidencia entre el vector normal unitario $\vec{N}$ y el vector hacia la fuente luminosa $\vec{L}$:
$$I_{\text{dif}} = K_d \cdot L_d \cdot \max(\vec{N} \cdot \vec{L}, 0)$$
* Cuando $\vec{N}$ y $\vec{L}$ son paralelos ($\vec{N} \cdot \vec{L} = 1$), la superficie recibe máxima iluminación. Conforme la superficie se aleja de la luz, el producto punto decae suavemente hacia 0, modelando la curvatura y volumen de la roca (M03).

### C. Componente Especular - Blinn-Phong ($I_{\text{esp}}$)
Modela los reflejos concentrados y brillantes sobre materiales pulidos. En la variante Blinn-Phong utilizada por OpenGL, se calcula el vector semimedio $\vec{H}$ entre el vector de luz $\vec{L}$ y el vector de visión $\vec{V}$:
$$\vec{H} = \frac{\vec{L} + \vec{V}}{\|\vec{L} + \vec{V}\|}$$
$$I_{\text{esp}} = K_s \cdot L_s \cdot \max(\vec{N} \cdot \vec{H}, 0)^{\text{shininess}}$$
* El exponente `GL_SHININESS` regula el decaimiento angular del brillo:
  - Valores altos ($90 - 100$): Concentran el destello en un área reducida y nítida (propio de metales bruñidos).
  - Valores bajos ($8 - 16$): Dispersan el reflejo en una franja ancha y tenue (propio de minerales y piedras).

### D. Observador Local (`GL_LIGHT_MODEL_LOCAL_VIEWER`)
Por defecto, OpenGL optimiza el cálculo especular asumiendo que el observador se halla en el infinito a lo largo del eje Z (`GL_FALSE`). Al activar:
```cpp
glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
```
OpenGL calcula la dirección real $\vec{V}$ desde cada vértice de la malla hacia la posición de la cámara, permitiendo que los reflejos especulares sobre superficies curvas (la roca de M03) y facetadas (la hoja de M01) varíen orgánicamente según la perspectiva del usuario.

### E. Normalización Automática (`GL_NORMALIZE`)
En el proyecto, varios módulos aplican `glScalef` (la hoja escala con factores $0.19 \times 1.65 \times 0.13$; la piedra con $1.30 \times 0.76 \times 1.03$). En transformaciones no uniformes, los vectores normales se deforman perdiendo su longitud unitaria ($\|\vec{N}\| \neq 1$). Sin normalización, los productos $\vec{N} \cdot \vec{L}$ se alteran drásticamente, quemando u oscureciendo la escena. Al activar:
```cpp
glEnable(GL_NORMALIZE);
```
la GPU normaliza automáticamente los vectores normales antes de evaluar las fórmulas de iluminación.

---

## 4. Arquitectura de Iluminación y Coordenadas de Mundo

Se configuró un esquema clásico de iluminación de dos fuentes luminosas puntuales ($w = 1.0$):

```
                                  [GL_LIGHT0: Luz Clave]
                                  Posicion: (-3.5, 7.0, 4.5)
                                  Tono calido medieval (0.92, 0.88, 0.80)
                                      \
                                       \
                                        v
                                  +------------+
                                  |  EXCALIBUR |
                                  |    (0,0,0) |
                                  +------------+
                                        ^
                                       /
                                      /
            [GL_LIGHT1: Luz de Relleno]
            Posicion: (4.5, 2.8, -3.8)
            Tono azul frio (0.22, 0.28, 0.40)
```

### Tabla de Configuración de Luces:
| Parámetro | Luz Principal (`GL_LIGHT0`) | Segunda Luz (`GL_LIGHT1`) |
|---|---|---|
| **Tipo** | Clave (Key Light) frontal alta | Relleno (Fill Light) contra-ángulo |
| **Posición Mundo ($X, Y, Z, W$)** | `(-3.5f, 7.0f, 4.5f, 1.0f)` | `(4.5f, 2.8f, -3.8f, 1.0f)` |
| **Componente Ambiente** | `(0.06f, 0.06f, 0.08f, 1.0f)` | `(0.00f, 0.00f, 0.00f, 1.0f)` |
| **Componente Difusa** | `(0.92f, 0.88f, 0.80f, 1.0f)` | `(0.22f, 0.28f, 0.40f, 1.0f)` |
| **Componente Especular** | `(1.00f, 0.98f, 0.92f, 1.0f)` | `(0.15f, 0.20f, 0.30f, 1.0f)` |
| **Atenuación** | $C=1.0,\; L=0.02,\; Q=0.001$ | $C=1.0,\; L=0.05,\; Q=0.000$ |

### Demostración: Estabilidad de la Luz en Coordenadas de Mundo
En OpenGL clásico, la posición de una fuente luminosa enviada con `glLightfv(..., GL_POSITION, P)` es multiplicada de inmediato por la matriz activa en `GL_MODELVIEW`:
$$P_{\text{ojo}} = M_{\text{modelview}} \cdot P$$

En el ciclo principal (`src/main.cpp`):
```cpp
void dibujarEscena() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    excalibur::camara::aplicar();             // Paso 1: Carga matriz de vista (LookAt)
    excalibur::graficos::aplicarIluminacion(); // Paso 2: Define posicion de la luz

    excalibur::objetos::dibujarEntorno();     // Paso 3: Dibuja objetos
    excalibur::objetos::dibujarPiedra();
    excalibur::objetos::dibujarHojaEspada();
    excalibur::objetos::dibujarEmpunadura();

    glutSwapBuffers();
}
```
Dado que en el **Paso 2** la matriz en `GL_MODELVIEW` es únicamente la matriz de vista de la cámara ($M_{\text{vista}}$):
$$P_{\text{ojo}} = M_{\text{vista}} \cdot P_{\text{mundo}}$$
La posición ingresada corresponde exactamente a coordenadas de **MUNDO**. Al cambiar la cámara de orientación o posición (vistas 1, 2, 3), la nueva matriz de vista vuelve a multiplicar las coordenadas de mundo, logrando que las fuentes luminosas permanezcan completamente inmóviles en el escenario y los reflejos se desplacen físicamente sobre la geometría.

---

## 5. Reflectancia de Materiales

Para coordinar con M01-M04 y garantizar que la escena tenga coherencia visual, se diseñaron cinco perfiles de material accesibles mediante `aplicarMaterial(Material)`:

```
+----------------------------------------------------------------------------------------------------+
| Material             | Especular (Ks)           | Shininess | Color Base Difuso (Kd) | Carácter    |
+----------------------------------------------------------------------------------------------------+
| MetalHoja (Acero)    | (0.92, 0.95, 1.00, 1.0)  |    96.0   | (0.78, 0.82, 0.88)     | Metálico    |
| MetalDorado (Guarda) | (0.88, 0.78, 0.38, 1.0)  |    64.0   | (0.88, 0.68, 0.22)     | Oro noble   |
| CueroMango (Mango)   | (0.08, 0.08, 0.08, 1.0)  |     6.0   | (0.35, 0.20, 0.12)     | Opaco mate  |
| Piedra (Roca)        | (0.15, 0.15, 0.16, 1.0)  |    12.0   | (0.42, 0.45, 0.46)     | Mineral     |
| Entorno (Terreno)    | (0.04, 0.04, 0.04, 1.0)  |     4.0   | (0.22, 0.30, 0.20)     | Tierra/pasto|
| Defecto (Neutro)     | (0.30, 0.30, 0.30, 1.0)  |    32.0   | Preserva actual        | Equilibrado |
+----------------------------------------------------------------------------------------------------+
```

### Características Visuales Logradas:
* **Hoja legible:** Especular al 95% y shininess 96 generan un destello metálico intenso a lo largo del filo y acanaladura, diferenciando el plano de la hoja respecto a la roca.
* **Piedra con volumen:** Especular de 15% y shininess 12 evitan el brillo ceroso o plástico, permitiendo que la variación de normales suaves (M06) modele hendiduras y crestas con fidelidad.

---

## 6. Contratos de Integración entre Módulos

| Módulo Relacionado | Tipo de Contrato | Detalle de Coordinación |
|---|---|---|
| **M06 (Normales)** | Dependencia de entrada | M09 requiere normales unitarias orientadas CCW. `GL_NORMALIZE` previene fallos ante escalas. |
| **M01 - M04 (Objetos)** | Entrega de materiales | M09 entrega funciones `aplicarMaterial(Material)` y `materialMetalHoja()`, `materialPiedra()`, etc. |
| **M08 (Cámara)** | Coordinación de orden | M09 exige que `aplicarIluminacion()` se invoque estrictamente tras `camara::aplicar()`. |
| **M10 (Integración)** | Controles y render | M09 expone `alternarLuzSecundaria()` para que M10 pueda vincular teclas de depuración. |

---

## 7. Banco de Pruebas Unitarias (`pruebas/m09`)

Se implementó un sistema de pruebas sin interfaz gráfica en [pruebas/m09/prueba_m09.cpp](file:///C:/Users/Junior%20duran/source/repos/Excalibur3D/pruebas/m09/prueba_m09.cpp) utilizando una emulación de llamadas de FreeGLUT ([stub/GL/freeglut.h](file:///C:/Users/Junior%20duran/source/repos/Excalibur3D/pruebas/m09/stub/GL/freeglut.h)).

### Comprobaciones Verificadas:
1. `GL_LIGHTING` y `GL_NORMALIZE` habilitados.
2. `GL_LIGHT0` habilitada y configurada con tono cálido medieval y atenuación suave.
3. `GL_LIGHT_MODEL_LOCAL_VIEWER` activado en modo observador local.
4. Luz ambiente global dentro del rango de equilibrio visual ($0.30 \le \sum RGB \le 0.90$).
5. Segunda luz `GL_LIGHT1` activa por defecto con tinte azul frío y sin ambiente parásito.
6. Posición de luces con componente homogénea $w = 1.0$ y colocadas en altura sobre la escena.
7. Material de la hoja con reflectancia especular $K_s \ge 0.90$ y exponente $Shininess \ge 90$.
8. Material de la piedra con reflectancia especular $K_s \le 0.20$ y exponente $Shininess \le 16$.
9. Alternancia y conmutación de estado con `alternarLuzSecundaria()`.

### Ejecución de Pruebas:
```cmd
cd pruebas\m09
cl /nologo /std:c++17 /W4 /EHsc /wd4505 /I stub /I ..\..\include prueba_m09.cpp ..\..\src\graficos\Iluminacion.cpp /Fe:prueba_m09.exe
prueba_m09.exe
```
**Resultado obtenido:** `== TODO OK (0 fallos) ==` (21 de 21 pruebas superadas con éxito).

---

## 8. Guía de Preguntas para la Defensa Oral

1. **¿Qué diferencia matemática hay entre la luz difusa y la luz especular?**  
   *Respuesta:* La luz difusa (Lambert) depende únicamente del ángulo entre la normal $\vec{N}$ y la dirección hacia la luz $\vec{L}$ ($\vec{N} \cdot \vec{L}$), dispersándose por igual en todas direcciones sin importar dónde esté el observador. La luz especular (Blinn-Phong) depende además de la posición del observador mediante el vector semimedio $\vec{H}$ ($\vec{N} \cdot \vec{H}$), elevado al exponente de brillo `GL_SHININESS`, produciendo un destello visible solo cuando el observador se alinea con la dirección de reflexión.

2. **¿Por qué las luces quedan fijas en el mundo y no se mueven con la cámara?**  
   *Respuesta:* Porque `glLightfv(..., GL_POSITION, pos)` se llama en `aplicarIluminacion()`, inmediatamente después de que `camara::aplicar()` carga la matriz de vista (`gluLookAt`) en `GL_MODELVIEW`. OpenGL multiplica las coordenadas pasadas por la matriz de vista actual; al contener exclusivamente la transformación de la cámara, esas coordenadas se interpretan como **coordenadas de mundo**.

3. **¿Por qué es indispensable activar `GL_NORMALIZE`?**  
   *Respuesta:* Porque los modelos aplican transformaciones de escala no uniformes con `glScalef`. Esto altera la magnitud de las normales ($\|\vec{N}\| \neq 1$), lo cual falsea los productos punto de Phong provocando artefactos oscuros o sobreexposición. `GL_NORMALIZE` fuerza a la GPU a renormalizar los vectores normales tras cada transformación geométrica.

4. **¿Cómo se calibraron los materiales de la hoja frente a la piedra?**  
   *Respuesta:* En la hoja de metal se configuró un coeficiente especular alto ($K_s = 0.95$) con un exponente de brillo elevado ($Shininess = 96$), logrando destellos nítidos que resaltan sus biseles. En la piedra se configuró un coeficiente especular bajo ($K_s = 0.15$) y un exponente amplio ($Shininess = 12$), produciendo un brillo suave y abierto que modela su volumen rugoso sin parecer plástico.

---

## 9. Referencias Bibliográficas
1. **Shreiner, D., Sellers, G., Kessenich, J.** (2017). *OpenGL Programming Guide: The Official Guide to Learning OpenGL (Red Book)*, 9th Edition. Addison-Wesley.
2. **De Vries, J.** (2020). *LearnOpenGL: Materials & Basic Lighting*. https://learnopengl.com/Lighting/Basic-Lighting
3. **Khronos Group**. *OpenGL 2.1 Reference Pages: glLight, glMaterial, glLightModel*. https://registry.khronos.org/OpenGL-Refpages/
