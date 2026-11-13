# Bitacora de investigacion - Modulo M09 (Iluminacion y Materiales)

**Proyecto:** Excalibur 3D  
**Equipo:** Grupo G5 - Integrante 9  
**Rama:** `feature/m09-iluminacion`  
**Fecha:** Noviembre 2026  

---

## Fuentes (titulo, autor y URL; por que son confiables)
1. **OpenGL Programming Guide (The Red Book), 9th Edition**  
   *Dave Shreiner, Graham Sellers, John Kessenich*  
   https://www.opengl.org/documentation/books/  
   *Por que es confiable:* Es la biblia oficial del consorcio Khronos Group sobre el pipeline de función fija y shaders de OpenGL, cubriendo la deducción del modelo de iluminación Phong y la interacción con matrices de transformación.
2. **LearnOpenGL: Colors, Basic Lighting & Materials**  
   *Joey de Vries*  
   https://learnopengl.com/Lighting/Basic-Lighting  
   *Por que es confiable:* Referencia didáctica estándar en la industria y recomendada explícitamente en la documentación de la asignatura para comprender el desglose ambiental, difuso y especular.
3. **OpenGL 2.1 Reference Pages: glLight, glMaterial, glLightModel**  
   *Khronos Group*  
   https://registry.khronos.org/OpenGL-Refpages/  
   *Por que es confiable:* Documentación oficial que especifica los efectos de cada parámetro, el cálculo con `GL_LIGHT_MODEL_LOCAL_VIEWER` y la multiplicación matricial en `GL_POSITION`.

---

## Problema 1: Configuración de CMake en Windows con Visual Studio (GLUT no encontrado)
- **Que no sabiamos / que pasaba:** El archivo `CMakeLists.txt` original usaba `find_package(GLUT REQUIRED)`. En Linux o MSYS2 funciona porque el paquete del sistema lo expone, pero en Windows con Visual Studio no existe FreeGLUT en los directorios del SDK de Microsoft, deteniendo la configuración con el error `Could NOT find GLUT (missing: GLUT_glut_LIBRARY)`. Además, CMake 4.3 exigía compatibilidad con políticas modernas.
- **Que probamos:** Configurar `find_package(GLUT)` como condicional y añadir un respaldo automático con `FetchContent` que descarga y compila FreeGLUT 3.4.0 de forma transparente, estableciendo `CMAKE_POLICY_VERSION_MINIMUM 3.5` y el flag `/EHsc` para MSVC.
- **Resultado:** Visual Studio configura y compila de forma limpia en un solo clic sin necesidad de instalaciones manuales de bibliotecas por parte de ningún integrante del equipo.

---

## Problema 2: Luz moviéndose al rotar o cambiar la cámara
- **Que no sabiamos / que pasaba:** Si la posición de la luz se establece antes de la transformación de vista o en el espacio del objeto, la luz rota solidaria con el punto de vista o con el modelo, generando un efecto de linterna frontal poco realista.
- **Que probamos:** Ubicar la llamada `glLightfv(GL_LIGHT0, GL_POSITION, pos)` en `aplicarIluminacion()`, ejecutada estrictamente después de `camara::aplicar()`.
- **Resultado:** Al tener `GL_MODELVIEW` cargada con la matriz de vista generada por `gluLookAt`, OpenGL premultiplica la posición y la ancla sólidamente a coordenadas de mundo. Al alternar las vistas 1, 2 y 3, la luz permanece totalmente fija y estable.

---

## Problema 3: Diferenciación de reflectancia entre la hoja y la piedra
- **Que no sabiamos / que pasaba:** Si se deja un material genérico con brillo alto, la roca parece plástico brillante o porcelana. Si se baja demasiado, la hoja de la espada pierde el destello característico del metal pulido.
- **Que probamos:** Definir perfiles diferenciados con `glMaterialfv` y `glMaterialf`:
  - Hoja: reflectancia especular $K_s = 0.95$, exponente de brillo 96.
  - Piedra: reflectancia especular $K_s = 0.15$, exponente de brillo 12.
  - Activación de `GL_LIGHT_MODEL_LOCAL_VIEWER` y `GL_NORMALIZE`.
- **Resultado:** La hoja presenta un destello metálico nítido en sus biseles, mientras que la piedra exhibe un gradiente difuso amplio que realza su relieve sin brillos sintéticos.

---

## Codigo ajeno reutilizado (un renglon por fragmento)
- `FetchContent_Declare(freeglut ...)`: Patrón oficial recomendado por CMake Discourse y Kitware para empaquetado de dependencias externas.
- Ecuaciones de atenuación cuadrática: Tomadas directamente de la especificación técnica de OpenGL 2.1 (`1.0 / (c + l*d + q*d^2)`).
