# Plan de 7 dias (propuesta)

- D1: validar requisitos con docente, compilar plantilla en todos los equipos,
  acuerdos sobre espacio/contratos, repartir ramas, abrir issues.
- D2: modelos minimos y primera prueba matematica SLERP sobre casos conocidos.
- D3: mallas de espada/piedra; UV sobre plano y roca; introducir camara interoperable.
- D4: **primera version obligatoria** con textura real, dos tipos de normales y SLERP.
- D5: mejoras de materiales, controles, diagnostico de culling y transiciones.
- D6: pruebas cruzadas, correcciones, bitacora con fuentes y fragmentos.
- D7: congelar main, ensayo de defensa y entrega.

## Orden de dependencias
- M01,M02,M03,M04 entregan geometria inicial para que M05 y M06 validen UV y normales.
- M05/M06 pueden trabajar antes con un triangulo, quad o esfera de prueba.
- M07 implementa SLERP en paralelo con M08, que puede usar una funcion temporal
  con la MISMA firma, sin simular que ya existe un SLERP real.
- M09 ilumina prototipos desde el primer dia y ajusta cuando haya normales.
- M10 administra API, compila todos los archivos, recibe PR y prueba.

## Definicion de hecho por modulo
- Compila en limpio en la rama.
- No cambia firmas ajenas de forma silenciosa.
- Tiene una demostracion visible o test que valide comportamiento.
- Incluye comentario explicativo y fuente si reutilizo material externo.
- PR pequeno con instrucciones para probar.
