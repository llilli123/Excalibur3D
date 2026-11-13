# Informe Técnico - Módulo M09: Iluminación y Materiales

**Responsable:** Integrante 9  
**Grupo:** G5  
**Rama:** `feature/m09-iluminacion`  
**Archivo propio:** `src/graficos/Iluminacion.cpp`  
**Cabecera vinculada:** `include/excalibur/Graficos.h`  

---

## 1. Resumen Ejecutivo
El módulo **M09** implementa el sistema de iluminación clásico de OpenGL basado en el modelo de reflexión de **Phong / Blinn-Phong**, definiendo las propiedades de luz del entorno y los perfiles de reflectancia de los materiales clave de la escena (*Excalibur 3D*): el **acero de la hoja**, el **oro de la guarda**, el **cuero del mango**, la **roca irregular** y el **terreno**.

Asimismo, se corrigió el error de configuración de **CMake** en Windows bajo Visual Studio (ausencia de GLUT en el sistema), dotando al proyecto de descarga y compilación automática de **FreeGLUT** mediante `FetchContent` sin alterar la compatibilidad con Linux y MSYS2.

---

## 2. Fundamentos Teóricos: Modelo de Iluminación Phong

En el pipeline de función fija de OpenGL, la intensidad luminosa $I$ en un punto de la superficie se calcula mediante la superposición de componentes:

$$I = I_{\text{emisión}} + I_{\text{ambiente\_global}} + \sum_{i} \left[ \text{att}_i \cdot \Big( I_{\text{amb}, i} + I_{\text{dif}, i} + I_{\text{esp}, i} \Big) \right]$$

### A. Componente Ambiental ($I_{\text{amb}}$)
Representa la luz indirecta que ha rebotado tantas veces en el entorno que carece de dirección aparente:
$$I_{\text{amb}} = K_a \cdot L_a$$
- **Luz ambiente global (`GL_LIGHT_MODEL_AMBIENT`):** Se configuró en `(0.16, 0.18, 0.22, 1.0)`. Proporciona un tono crepuscular tenue que evita tanto que las zonas no iluminadas queden en negro absoluto como que la escena sufra de falta de contraste ("efecto lavado").

### B. Componente Difusa - Ley de Lambert ($I_{\text{dif}}$)
Modela la reflexión omnidireccional en superficies ásperas o rugosas. Depende exclusivamente del ángulo entre la normal de la superficie $\vec{N}$ y el vector hacia la fuente de luz $\vec{L}$:
$$I_{\text{dif}} = K_d \cdot L_d \cdot \max(\vec{N} \cdot \vec{L}, 0)$$
- Da volumen y modelado visual a la roca irregular (M03) y caras de la espada (M01).

### C. Componente Especular ($I_{\text{esp}}$)
Modela el brillo brillante y concentrado característico de superficies pulidas o metálicas. Se evalúa con el vector semimedio $\vec{H} = \frac{\vec{L} + \vec{V}}{\|\vec{L} + \vec{V}\|}$ (Blinn-Phong) o el vector reflejado $\vec{R}$ (Phong):
$$I_{\text{esp}} = K_s \cdot L_s \cdot \max(\vec{N} \cdot \vec{H}, 0)^{\text{shininess}}$$
- El parámetro `GL_SHININESS` controla la concentración del reflejo especular. Valores altos ($>80$) generan destellos diminutos y nítidos propios del metal bruñido, mientras que valores bajos ($<20$) generan dispersión suave ideal para piedra.

### D. Observador Local (`GL_LIGHT_MODEL_LOCAL_VIEWER`)
Por defecto en OpenGL, el vector de visión $\vec{V}$ se asume paralelo al eje Z infinito. Al activar `glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE)`, OpenGL calcula la dirección exacta desde cada vértice hacia la posición de la cámara, logrando que los reflejos especulares se curven de forma natural y realista sobre la hoja y la roca.

---

## 3. Arquitectura de Luces y Coordenadas de Mundo

Se implementó un esquema de dos luces posicionales ($w = 1.0$):

| Parámetro | Luz Principal (`GL_LIGHT0`) | Luz de Relleno (`GL_LIGHT1`) |
|---|---|---|
| **Rol** | Luz clave (Key Light) frontal-alta | Luz de contra-ángulo (Fill Light) mística |
| **Posición Mundo** | `(-3.5, 7.0, 4.5, 1.0)` | `(4.5, 2.8, -3.8, 1.0)` |
| **Ambiente** | `(0.06, 0.06, 0.08, 1.0)` | `(0.00, 0.00, 0.00, 1.0)` |
| **Difusa** | `(0.92, 0.88, 0.80, 1.0)` (cálido natural) | `(0.22, 0.28, 0.40, 1.0)` (azul frío nocturno) |
| **Especular** | `(1.00, 0.98, 0.92, 1.0)` (blanco-dorado puro) | `(0.15, 0.20, 0.30, 1.0)` (destello azul tenue) |
| **Atenuación** | Const: 1.0, Lin: 0.02, Cuad: 0.001 | Const: 1.0, Lin: 0.05 |

### Estabilidad ante Cambios de Cámara
Un error habitual en OpenGL consiste en definir la posición de la luz antes de aplicar la matriz de vista (quedando la luz anclada a la cámara como una linterna de minero) o dentro del bucle de cada objeto.

En este diseño:
```cpp
// main.cpp:
excalibur::camara::aplicar();             // 1. Carga la matriz de vista en GL_MODELVIEW
excalibur::graficos::aplicarIluminacion(); // 2. Fija las luces
```
Al invocar `glLightfv(GL_LIGHT0, GL_POSITION, pos)` inmediatamente después de `camara::aplicar()`, OpenGL premultiplica el vector `pos` por la matriz de vista:
$$P_{\text{ojo}} = M_{\text{vista}} \cdot P_{\text{mundo}}$$
Esto ancla sólidamente las luces a las coordenadas de **mundo**. Al cambiar de vista (teclas 1, 2, 3) o interpolar con SLERP, las fuentes luminosas se perciben completamente fijas en la escena y los reflejos se mueven coherentemente sobre los objetos.

---

## 4. Perfiles de Materiales y Reflectancia

Se diseñaron perfiles específicos mediante `glMaterialfv` y `glMaterialf`:

```
+---------------------------------------------------------------------------------+
| Material            | Especular (Ks)       | Shininess | Tono Difuso (Kd)       |
+---------------------------------------------------------------------------------+
| MetalHoja (Acero)   | (0.92, 0.95, 1.00)   |   96.0    | (0.78, 0.82, 0.88)     |
| MetalDorado (Oro)   | (0.88, 0.78, 0.38)   |   64.0    | (0.88, 0.68, 0.22)     |
| CueroMango          | (0.08, 0.08, 0.08)   |    6.0    | (0.35, 0.20, 0.12)     |
| Piedra (Roca)       | (0.15, 0.15, 0.16)   |   12.0    | (0.42, 0.45, 0.46)     |
| Entorno (Suelo)     | (0.04, 0.04, 0.04)   |    4.0    | (0.22, 0.30, 0.20)     |
+---------------------------------------------------------------------------------+
```

- **Hoja legible:** Con reflectancia especular del 95% y exponente 96, los reflejos delinean claramente la arista central y los filos cortantes sin quemar los colores base.
- **Piedra con volumen:** Con reflectancia de apenas 15% y exponente 12, se resalta la curvatura y las irregularidades sin producir brillos plásticos indeseados.

---

## 5. Prevención de Errores Habituales
1. **Falta de `GL_NORMALIZE` tras escalado:**  
   Al escalar objetos con `glScalef` (como hace la hoja $0.19 \times 1.65 \times 0.13$ o la piedra $1.30 \times 0.76 \times 1.03$), la longitud de los vectores normales se distorsiona, multiplicando erróneamente la intensidad de la luz difusa y especular. Se aseguró `glEnable(GL_NORMALIZE)` tanto en la inicialización de M09 como en el pipeline de `main.cpp`.
2. **Luz moviéndose con la cámara:**  
   Resuelto mediante el orden estricto de invocación en `main.cpp` tras `camara::aplicar()`.
3. **Escena sobreexpuesta u oscura:**  
   Verificado matemáticamente con coeficientes acotados: la suma de componentes ambiental y difusa no satura los canales RGB.

---

## 6. Pruebas de Aceptación Automatizadas (`pruebas/m09`)
Se elaboró un banco de pruebas unitarias (`pruebas/m09/prueba_m09.cpp`) sobre FreeGLUT simulado que verifica 21 condiciones críticas sin necesidad de GPU ni ventana gráfica:
- Estado del pipeline OpenGL (`GL_LIGHTING`, `GL_NORMALIZE`, `GL_LIGHT0`, `GL_LIGHT1`, `LOCAL_VIEWER`).
- Parámetros cromáticos y balance de luminancia.
- Carácter posicional 4D ($w = 1.0$) en coordenadas de mundo.
- Diferenciación de reflectancia entre acero pulido ($K_s \ge 0.90, S \ge 90$) y roca rugosa ($K_s \le 0.20, S \le 16$).
- Alternancia dinámica de la segunda luz en caliente.

**Resultado:** `== TODO OK (0 fallos) ==` (21 de 21 comprobaciones superadas).
