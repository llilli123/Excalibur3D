# Guía de Estudio para la Defensa Oral - Módulo M09

**Módulo:** M09 - Iluminación y Materiales (Modelo Phong)  
**Proyecto:** Excalibur 3D (Computación Gráfica II)  
**Preguntas típicas de examen/defensa y respuestas modelo con fundamentación técnica.**

---

### 1. ¿Cómo funciona la ecuación de iluminación de Phong en OpenGL clásico?
**Respuesta clave:**  
El modelo clásico calcula el color en cada vértice sumando tres componentes principales:
1. **Ambiente:** Luz difusa dispersa constante en todas las direcciones ($K_a \cdot L_a$). No depende de la posición del observador ni de la normal.
2. **Difusa (Lambert):** Reflexión en superficies rugosas que depende del coseno del ángulo entre la normal del vértice $\vec{N}$ y la dirección hacia la luz $\vec{L}$ ($\vec{N} \cdot \vec{L}$). Es máxima cuando la luz incide perpendicular a la superficie.
3. **Especular (Blinn-Phong):** Reflejo brillante concentrado. En OpenGL clásico con Blinn-Phong, depende de la normal $\vec{N}$ y el vector semimedio $\vec{H}$ ($\vec{N} \cdot \vec{H}$), elevado al exponente de brillo `GL_SHININESS`.

---

### 2. ¿Cómo lograste que la luz quede fija en el mundo y no se mueva con la cámara?
**Respuesta clave:**  
En OpenGL, cuando se llama a `glLightfv(GL_LIGHT0, GL_POSITION, pos)`, las coordenadas proporcionadas son multiplicadas automáticamente por la matriz actual en `GL_MODELVIEW` para transformarlas al espacio de la cámara (eye coordinates).

Por ello, en nuestro proyecto:
1. La función `excalibur::camara::aplicar()` carga la matriz de vista en `GL_MODELVIEW` usando `gluLookAt`.
2. Inmediatamente después, `excalibur::graficos::aplicarIluminacion()` envía la posición de la luz.
3. Al existir en `GL_MODELVIEW` únicamente la transformación de la cámara, las coordenadas que pasamos son interpretadas como **coordenadas de mundo**.
4. Si hubiésemos puesto la luz antes de la cámara, quedaría fija en coordenadas de ojo (efecto "linterna en la frente"). Si la pusiéramos dentro de la matriz de un objeto, se movería con ese objeto.

---

### 3. ¿Por qué es fundamental activar `GL_NORMALIZE` en esta escena?
**Respuesta clave:**  
Varios módulos utilizan `glScalef` para dimensionar sus objetos (por ejemplo, la hoja de la espada se escala con factores como $0.19$ en X y $1.65$ en Y; la piedra se escala $1.30 \times 0.76 \times 1.03$).  
Cuando se aplica una transformación de escala no uniforme, las normales geométricas se deforman y pierden su longitud unitaria. Como los productos punto de Phong ($\vec{N} \cdot \vec{L}$ y $\vec{N} \cdot \vec{H}$) asumen que $\|\vec{N}\| = 1$, si la normal se acorta o se alarga, los cálculos de luz difusa y especular se alteran radicalmente, produciendo manchas negras o quemados de luz.  
Activar `glEnable(GL_NORMALIZE)` le ordena a la GPU re-normalizar automáticamente cada vector normal después de la transformación geométrica antes de calcular la luz.

---

### 4. ¿Qué diferencia visual y técnica hay entre el material de la hoja de la espada y el de la piedra?
**Respuesta clave:**  
Se diferencian en dos parámetros de `glMaterial`:
1. **Reflectancia especular (`GL_SPECULAR`):**  
   - Para la **hoja de metal pulido**, el coeficiente especular es muy alto (`0.92, 0.95, 1.00`), reflejando casi el 100% de la luz incidente para simular acero reflectante.
   - Para la **piedra**, el coeficiente especular es bajo (`0.15, 0.15, 0.16`), reflejando solo una fracción sutil de luz.
2. **Exponente de brillo (`GL_SHININESS`):**  
   - En la **hoja**, `GL_SHININESS = 96.0`, lo que concentra el brillo en un destello diminuto y muy nítido a lo largo del filo y los biseles.
   - En la **piedra**, `GL_SHININESS = 12.0`, lo que genera un brillo disperso y ancho que define el relieve y curvatura de la roca sin dar apariencia de plástico brillante.

---

### 5. ¿Qué hace `GL_LIGHT_MODEL_LOCAL_VIEWER` y por qué lo activaron?
**Respuesta clave:**  
Por defecto en OpenGL, el cálculo del reflejo especular asume que el observador se encuentra infinitamente lejos a lo largo del eje Z (`GL_FALSE`), lo cual simplifica los cálculos pero genera reflejos poco realistas cuando la cámara está cerca de la escena.  
Al activar `glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE)`, OpenGL calcula el vector hacia el observador $\vec{V}$ de manera exacta para cada vértice a partir de su posición real relativa a la cámara. Esto hace que los reflejos especulares sobre la superficie convexa de la piedra y los planos inclinados de la hoja se vean curvos, correctos y con volumen verosímil.

---

### 6. ¿Por qué incluyeron una segunda luz (`GL_LIGHT1`) y qué aporta a la composición visual?
**Respuesta clave:**  
Se implementó como **luz de relleno (Fill Light)** en contra-ángulo (`4.5, 2.8, -3.8`).  
Una sola luz puntual fuerte genera sombras duras y caras traseras completamente oscuras donde se pierde la forma de la roca y de la espada. La segunda luz utiliza un tono azul frío tenue con atenuación y sin componente ambiental propia, lo que ilumina suavemente las caras en penumbra emulando luz ambiental o de luna mística, revelando los volúmenes sin deslumbrar ni sobreexponer la escena.
