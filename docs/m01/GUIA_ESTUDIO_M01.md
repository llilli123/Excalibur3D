# Guía de estudio · M01 Hoja de Excalibur

> Documento personal para preparar la defensa oral y el segundo parcial.
> Archivo del módulo: `src/objetos/EspadaHoja.cpp` · Rama: `feature/m01-espada-hoja`

**Regla del curso:** si no puedes explicar un fragmento, no lo puedes usar. Esta guía recorre
el código parte por parte. Lee cada sección con el archivo abierto al lado.

---

## 1. Tu módulo en 30 segundos

"Mi módulo es la hoja de la espada. Es una malla propia de **502 triángulos** construida a mano:
una sección transversal de 14 puntos (filos, biseles, cara plana y una acanaladura central) que
se repite en 18 alturas siguiendo un perfil de hoja de laurel y termina en una punta ojival.
Cada triángulo lleva **una normal por cara**, calculada con el producto cruz, porque el metal
forjado es plano y anguloso: así se ven los cantos. Por eso la hoja es el **objeto facetado**
del requisito R2. Encima dibujo efectos (runas, aura, chispas y luz de contorno) con mezcla
aditiva, y restauro todo el estado de OpenGL al terminar para no afectar a los demás módulos."

---

## 2. Conceptos que tienes que dominar

### 2.1 Malla y triángulos
Una **malla** es un conjunto de vértices (puntos 3D) agrupados en triángulos. OpenGL solo
sabe dibujar puntos, líneas y triángulos; cualquier superficie (una hoja, una roca) se
aproxima con triángulos. En el código la malla usa la estructura `graficos::Malla` del contrato:
`posiciones`, `normales`, `uv` e `indices` (3 índices por triángulo).

### 2.2 Winding (orden de los vértices) y CCW
El **winding** es el orden en que se dan los 3 vértices de un triángulo. Si, visto desde la
cámara, el orden es **antihorario (CCW, counter-clockwise)**, OpenGL considera que ves la
**cara frontal**. Lo configura `glFrontFace(GL_CCW)` (en `Mallas.cpp`, módulo M06).

En la hoja, los 14 puntos de cada sección están ordenados así: filo derecho (+X), cara trasera
(−Z) de derecha a izquierda, filo izquierdo (−X), cara frontal (+Z) de izquierda a derecha.
Con ese orden, al unir dos secciones con `(abajo[i], abajo[j], arriba[j])` cada cara queda CCW
vista **desde fuera**.

### 2.3 Back-face culling
Con `GL_CULL_FACE` activo y `glCullFace(GL_BACK)`, OpenGL **no dibuja** los triángulos que
se ven en sentido horario (las caras traseras). Ahorra trabajo porque en un objeto cerrado las
caras traseras nunca se ven. **Riesgo:** si un triángulo tiene el winding al revés, desaparece
desde el lado bueno y aparece un "agujero". La prueba `volumen positivo` y `malla cerrada`
verifica que eso no pasa en la hoja.

### 2.4 Normal de una cara con el producto cruz
La **normal** es un vector perpendicular a la superficie, de largo 1, que apunta hacia fuera.
La luz se calcula con ella (cuanto más mira la normal hacia la luz, más iluminada está la cara).

Para un triángulo `a, b, c` en orden CCW:

```
n = normalizar( (b − a) × (c − a) )
```

Ejemplo para practicar: `a = (0,0,0)`, `b = (1,0,0)`, `c = (0,1,0)`
- `b − a = (1,0,0)`, `c − a = (0,1,0)`
- `(1,0,0) × (0,1,0) = (0·0 − 0·1, 0·0 − 1·0, 1·1 − 0·0) = (0, 0, 1)`
- La normal apunta a +Z, hacia quien mira el triángulo en orden antihorario. ✔

Si intercambias `b` y `c`, el resultado es `(0,0,−1)`: **el orden de los vértices decide hacia
dónde apunta la normal**. Por eso winding y normales están conectados.

En el código: `graficos::normalDeCara(a, b, c)` (función de M06) dentro de `agregarTriangulo`.

### 2.5 Normal por cara (facetado) vs. normal por vértice (suave) ← pregunta del parcial
| | Normal por **cara** | Normal por **vértice** |
|---|---|---|
| Cómo se calcula | Producto cruz del triángulo | Promedio de las normales de las caras que tocan el vértice |
| Vértices | No se comparten entre caras (cada triángulo tiene los suyos) | Se comparten |
| Aspecto | Cada cara de un tono uniforme, se ven los cantos | Transición suave, sin cantos |
| Para qué | Superficies planas y angulosas: **hoja de metal**, cristal, cubo | Superficies curvas: **roca**, esfera, jarrón |

**Respuesta modelo:** "La hoja lleva normal por cara porque es metal forjado con superficies
planas y aristas vivas: el filo, el bisel y la acanaladura son planos que se cortan en ángulo.
Con una normal por cara, cada plano recibe un tono uniforme y se ven las aristas. Si promediara
las normales por vértice, OpenGL suavizaría la luz entre caras y la arista del bisel se vería
redondeada, como plástico. La roca, en cambio, es curva y usa normales por vértice."

**Detalle fino:** el programa usa `glShadeModel(GL_SMOOTH)`, que interpola los valores entre
vértices. ¿Por qué entonces la hoja se ve facetada? Porque los **tres vértices de cada triángulo
tienen la misma normal**: interpolar tres valores iguales da el mismo valor. Lo que sí varía
dentro de una cara es el **color** (filo claro, canal azulado); eso es solo tinte, no sombreado.

Imagen: `img/hoja_normales_por_cara.png` (cada línea amarilla es la normal de una cara;
compilar con `-DEXCALIBUR_DEBUG_NORMALES`).

### 2.6 Coordenadas UV
Una coordenada **UV** dice qué punto de una imagen (textura) corresponde a cada vértice.
`u` y `v` van de 0 a 1: (0,0) es una esquina de la imagen y (1,1) la opuesta. Entre vértices,
OpenGL interpola las UV. Si las UV están mal asignadas, la imagen se estira, se comprime o se
repite.

En la hoja (por si M05 la texturiza): `u` va de filo a filo y `v` va de la punta (0) a la base
(1). Función `uvDe(p)`. La prueba verifica que todas quedan dentro de [0, 1].

### 2.7 Iluminación: difusa y especular
- **Difusa:** depende del ángulo entre la normal y la luz. Da el volumen.
- **Especular:** el reflejo brillante; depende de la cámara. `GL_SHININESS` controla qué tan
  concentrado es (más alto = brillo más pequeño y nítido, como metal pulido).
- En la hoja: `glMaterialfv(GL_FRONT, GL_SPECULAR, ...)` y `GL_SHININESS = 96` solo mientras se
  dibuja la hoja (el resto de la escena conserva lo que definió M09).
- `GL_COLOR_MATERIAL` (activado en `main.cpp`) hace que `glColor3f` defina el color ambiente y
  difuso del material: por eso el color por vértice funciona con la luz.

### 2.8 Guardar y restaurar estado (glPushMatrix / glPushAttrib)
OpenGL es una **máquina de estados**: si activas la mezcla y no la apagas, todo lo que se dibuje
después también se mezcla. El contrato del equipo dice: "no dejar estados OpenGL alterados".
- `glPushMatrix / glPopMatrix` guarda y restaura la matriz de transformación.
- `glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT |
  GL_LINE_BIT | GL_POINT_BIT | GL_CURRENT_BIT)` guarda luz, mezcla, profundidad, grosor de línea
  y punto, y color actual. `glPopAttrib()` los restaura al final de `dibujarHojaEspada()`.

### 2.9 Mezcla aditiva y profundidad (efectos)
- `glBlendFunc(GL_SRC_ALPHA, GL_ONE)`: el color del efecto se **suma** al que ya está en
  pantalla, multiplicado por su alfa. Sumar luz solo puede aclarar, por eso parece que brilla.
- `glDepthMask(GL_FALSE)`: los efectos no escriben en el buffer de profundidad, así no tapan
  otros objetos. Pero la prueba de profundidad sigue activa, así que **la roca sí tapa a los
  efectos** (por eso el resplandor parece salir de la grieta).
- `glDisable(GL_LIGHTING)`: los efectos "emiten" luz propia; no los sombrea la lámpara.

### 2.10 Posición de la cámara desde la matriz MODELVIEW (luz de contorno)
La matriz MODELVIEW es `[R | T]` (rotación R y traslación T). Lleva puntos del mundo a la
cámara. La cámara está donde ese resultado da el origen: `R·ojo + T = 0` → `ojo = −Rᵀ·T`.
OpenGL guarda la matriz por columnas, así que `Rᵀ` se lee por filas del arreglo. Función
`posicionCamara()`.

**Luz de contorno (rim light):** `intensidad = (1 − |n·v|)³`, donde `v` es la dirección de la
cara a la cámara. Si la cara mira de frente a la cámara, `n·v ≈ 1` → no brilla. Si se ve de
canto, `n·v ≈ 0` → brilla al máximo. El cubo concentra el brillo solo en los bordes. Es una
versión simplificada de la aproximación de Fresnel–Schlick.

---

## 3. Recorrido del código (en orden)

| Parte | Qué hace | Qué decir si te preguntan |
|---|---|---|
| `EXCALIBUR_Y_GUARDA_INFERIOR`, `Y_BASE`, `Y_PUNTA` | Medidas del contrato: base dentro de la guarda (2.85), punta dentro de la roca (1.17) | "Coordiné con M02 y M03; si sube la guarda, cambio una constante" |
| `S_ANILLOS` | 18 alturas relativas, más juntas cerca de la punta | "Ahí la curva cambia más rápido, necesito más cortes" |
| `perfil(s)` | Ancho, grosor y canal en cada altura. `s=0` punta, `s=1` base | Ver 3.1 |
| `suave(a,b,x)` | *smoothstep*: transición suave de 0 a 1 | "Para que el canal nazca y muera sin escalón" |
| `seccion(anillo)` | Los 14 puntos de un corte, en orden CCW | Dibujar el rombo con canal en la pizarra |
| `construirHoja()` | Une anillos con 2 triángulos por segmento, cierra punta y tapa | "Cuerpo 17×14×2 + punta 14 + tapa 12 = 502" |
| `agregarTriangulo()` | Calcula la normal de cara y guarda 3 vértices propios | "No comparto vértices: así la normal es por cara" |
| `dibujarMallaIluminada()` | La pasada sólida con luz (la que demuestra R2) | `glNormal3f` + `glColor3f` + `glTexCoord2f` + `glVertex3f` |
| `dibujarEfectos()` | Prepara mezcla aditiva y llama a cada efecto | Ver 2.9 |
| `dibujarHojaEspada()` | Función pública: guarda estado, dibuja, restaura | Ver 2.8 |

### 3.1 La función `perfil(s)`
- **Ancho (laurel):** de `s = 0.28` hacia arriba, `0.058 + 0.014·u + 0.009·sin(π·u)`: la parte
  lineal hace la hoja un poco más ancha en la base y el seno la abomba a media hoja.
- **Punta ojival:** debajo de `s = 0.28`, `ancho = 0.058·(s/0.28)^0.6`. El exponente < 1 hace
  que se cierre en curva y no en cono recto.
- **Distal taper:** el grosor es 0.026 en la mitad superior y baja hacia la punta (en una espada
  real la hoja es más gruesa cerca del mango para resistir).
- **Canal:** `k = suave(0.30,0.40,s)·(1 − suave(0.93,0.985,s))` vale 0 en la base, 1 a media
  hoja y 0 antes de la punta. La profundidad y el ancho del canal se multiplican por `k`.
  En la base `k = 0`, la sección es convexa y la tapa se cierra con un abanico sin solaparse.

### 3.2 ¿Cuántos triángulos y por qué?
- Cuerpo: 17 tramos entre 18 anillos × 14 segmentos × 2 triángulos = **476**
- Punta: 14 triángulos que convergen en un vértice = **14**
- Tapa superior: abanico de 14 puntos = **12**
- **Total: 502** (lo confirma la prueba automática)

---

## 4. Las preguntas del parcial (respuestas para memorizar y entender)

**¿Por qué este objeto lleva normal por vértice y aquel por cara?**
Ver 2.5. Hoja = facetado porque es plana y angulosa. Roca = suave porque es curva.

**¿Qué pasaría si el `near` de la proyección fuera 0?**
La proyección en perspectiva divide entre la profundidad y usa `far/near` para repartir la
precisión del buffer de profundidad. Con `near = 0` la matriz queda degenerada (división entre
cero / relación infinita): se pierde toda la precisión de profundidad y los objetos se dibujan
encima de otros incorrectamente (*z-fighting*), o no se dibuja nada. Por eso `near` debe ser
positivo; el proyecto usa `0.1` (`gluPerspective(60, aspecto, 0.1, 100)`).

**¿Por qué un cuaternión evita el gimbal lock?**
Con ángulos de Euler la orientación se arma con tres giros en secuencia; si uno de ellos llega a
90°, dos ejes quedan alineados y se pierde un grado de libertad (*gimbal lock*). Un cuaternión
representa la orientación como **un solo giro** alrededor de un eje arbitrario (4 números
`w, x, y, z` de norma 1), sin secuencia de ejes, así que no hay ejes que se alineen. Además
`slerp` interpola sobre la esfera de orientaciones con velocidad angular constante.

**¿Para qué sirven las UV?** Ver 2.6. **¿Qué hace el culling?** Ver 2.3.

---

## 5. Preguntas probables sobre TU código

1. **¿Por qué tus vértices no se comparten?** Porque un vértice compartido tendría una sola
   normal; para normal por cara cada triángulo necesita sus propios vértices.
2. **¿Cómo sabes que ninguna cara está al revés?** Por la prueba automática: la normal enviada
   coincide con `(b−a)×(c−a)`, el volumen con signo es positivo (las caras miran hacia fuera)
   y cada arista aparece en dos caras con sentidos opuestos (malla cerrada).
3. **¿Por qué hay una tapa arriba si no se ve?** Para que la malla sea cerrada. Si M02 cambia la
   guarda o se mira en modo alambre, no aparece un hueco.
4. **¿Por qué `glPushAttrib`?** Para no dejar la mezcla o la luz cambiadas para la empuñadura,
   que se dibuja después (contrato del equipo).
5. **¿Por qué la hoja se ve facetada si `GL_SMOOTH` está activo?** Ver el "detalle fino" de 2.5.
6. **¿Los efectos rompen el requisito R2?** No: la pasada sólida con normales por cara se dibuja
   primero; los efectos se suman encima. Con `-DEXCALIBUR_HOJA_SIN_EFECTOS` se ve solo el
   facetado (`img/hoja_facetada_sin_efectos.png`).
7. **¿Cómo se anima sin guardar estado?** Todo depende de `glutGet(GLUT_ELAPSED_TIME)`: la
   posición de cada chispa o el brillo de cada runa es una función del tiempo.

---

## 6. Cómo compilar y probar

```bash
cmake -S . -B build
cmake --build build
```

Pruebas automáticas del módulo (usan un FreeGLUT simulado, no necesitan ventana). Ver
`pruebas/m01/README.md`.

Variantes útiles para la defensa:
- `-DEXCALIBUR_HOJA_SIN_EFECTOS` → solo la malla facetada.
- `-DEXCALIBUR_DEBUG_NORMALES` → dibuja la normal de cada cara.

Teclas: `1/2/3` vistas, `P` alambre, `ESC` salir.

---

## 7. Autoevaluación (hazla sin mirar)

1. Calcula la normal de `a=(0,0,0), b=(0,1,0), c=(1,0,0)`. ¿Hacia dónde apunta y por qué?
2. ¿Qué pasa con la hoja si invierto el orden de `agregarTriangulo(h, abajo[i], abajo[j], arriba[j])`?
3. Explica en una frase por qué la roca usa normales por vértice.
4. ¿Por qué el resplandor de la entrada no tapa la roca, pero la roca sí tapa el resplandor?
5. Si M02 sube la guarda 0.6, ¿qué cambias en tu archivo?

<details><summary>Respuestas</summary>

1. `(0,1,0)×(1,0,0) = (0,0,−1)`: apunta a −Z porque el orden es horario visto desde +Z.
2. Todas las caras quedarían con winding horario: el culling las descartaría y la hoja se
   vería "por dentro" o desaparecería; además las normales apuntarían hacia dentro.
3. Porque es curva y el promedio de normales suaviza la luz entre caras vecinas.
4. Porque los efectos no escriben profundidad (`glDepthMask(GL_FALSE)`), pero sí la consultan.
5. `EXCALIBUR_Y_GUARDA_INFERIOR` a `3.435f`. El resto se recalcula solo.
</details>
