Excalibur 3D | La espada en la piedra
Proyecto de Computación Gráfica II desarrollado en C++17, OpenGL clásico y FreeGLUT. Representa una escena medieval interactiva con la espada Excalibur incrustada en una piedra, un paisaje natural y controles para explorar la escena, extraer la espada y reproducir música ambiental.

El proyecto está organizado en 10 módulos principales, con mejoras posteriores de ambiente e interacción. La rama development concentra la integración de los trabajos del equipo.

Repositorio: https://github.com/llilli123/Excalibur3D
Rama de integración: development
	
Funcionalidades implementadas
- Espada 3D: hoja modelada con geometría propia y superficies facetadas; empuñadura con guarda, mango y pomo.
- Piedra irregular: malla con normales suaves y textura de roca.
- Terreno y ambientación: suelo texturizado, vegetación, rocas, conejos, montañas lejanas, pinos, horizonte y niebla.
- Agua: anillo alrededor de la piedra y canales con orillas, transparencias y variaciones animadas de brillo.
- Texturas UV: carga de los archivos BMP de roca y suelo, con coordenadas UV y repetición en el terreno.
- Iluminación y materiales: luz principal, segunda luz alternable y materiales diferenciados para metal, cuero, piedra y entorno.
- Cámara 3D: tres vistas con transiciones suaves de orientación mediante cuaterniones y SLERP.
- Interacción con la espada: extracción y devolución mediante una traslación compartida para hoja y empuñadura, con animación suavizada.
- Audio ambiental: música WAV en bucle, activable desde el teclado en Windows.
- Herramientas de visualización: modo alambre para inspeccionar la geometría y configuración de descarte de caras posteriores (back-face culling).

#Controles
Tecla	Acción
1	Seleccionar la primera vista de cámara
2	Seleccionar la vista lateral
3	Seleccionar la vista superior
P	Alternar entre superficie sólida y modo alambre (wireframe)
L	Activar o desactivar la segunda luz
E	Extraer Excalibur de la piedra
R	Devolver Excalibur a la piedra
M	Activar o detener la música ambiental (Windows)
Esc	Cerrar la aplicación


Las teclas alfabéticas admiten mayúsculas y minúsculas. Las transiciones de cámara y la animación de la espada se actualizan desde el temporizador de FreeGLUT.
Tecnologías y requisitos
- C++17.
- CMake 3.16 o superior, con la compatibilidad configurada en el CMakeLists.txt del proyecto.
- OpenGL y GLU.
- FreeGLUT para crear la ventana y recibir eventos de teclado.
- Un compilador C++ compatible: por ejemplo, MinGW-w64 o MSVC en Windows, o GCC en Linux.
- Git y acceso a internet durante la primera configuración en Windows, porque CMake obtiene FreeGLUT 3.4.0 mediante FetchContent.
Windows: CLion, MinGW o Visual Studio
En Windows, el CMakeLists.txt descarga y compila FreeGLUT 3.4.0 como biblioteca estática, por lo que el ejecutable no necesita una DLL separada de FreeGLUT. Cuando se utiliza MinGW, también intenta copiar junto al .exe las DLL de ejecución que encuentre en la carpeta del compilador (libgcc_s_seh-1.dll, libstdc++-6.dll y libwinpthread-1.dll).

Importante: no es necesario colocar DLL manualmente en C:\Windows\System32. Si una dependencia falta, comprueba que CLion use la instalación correcta del compilador. La configuración se ha utilizado en CLion con MinGW; otros entornos deben comprobarse en sus propios equipos.
Linux (Debian/Ubuntu)
Instalar las herramientas y bibliotecas de desarrollo, por ejemplo:
sudo apt update
sudo apt install build-essential cmake git freeglut3-dev libglu1-mesa-dev
En Linux, CMake utiliza la instalación de GLUT disponible en el sistema. El código de audio no reproduce música fuera de Windows; el resto del proyecto está preparado para compilar con las dependencias correspondientes, pero debe verificarse en el equipo de destino.
Descarga y compilación
1. Clonar la rama integrada
git clone --branch development https://github.com/llilli123/Excalibur3D.git
cd Excalibur3D
2. Configurar y compilar con CMake
cmake -S . -B build
cmake --build build
En Windows, la primera configuración puede tardar porque CMake descarga y construye FreeGLUT. Si utilizas CLion, abre la carpeta raíz del proyecto, configura el compilador y ejecuta Reload CMake Project y Build.

3. Ejecutar
Linux o Windows con generador de configuración única:
./build/Excalibur3D
En Windows, el ejecutable se llama Excalibur3D.exe; normalmente estará en build/ o, si usas un generador multiconfiguración, en build/Debug/. En CLion también puede estar en cmake-build-debug/.

La ubicación exacta depende del generador y de la configuración de CMake.
Problemas habituales
- Error 0xC0000135 en Windows: falta una DLL de ejecución. Verifica el toolchain de MinGW, recompila y comprueba las DLL junto al ejecutable.
- No se descarga FreeGLUT: revisa la conexión a internet y que Git esté disponible para CMake.
- Texturas de reemplazo o ausentes: comprueba assets/texturas/roca.bmp y assets/texturas/suelo.bmp y la ruta definida por EXCALIBUR_ASSETS_DIR.
- La música no suena: se requiere Windows y assets/audio/ambiente.wav. La reproducción utiliza la API WinMM.
- Geometría invisible o iluminación extraña: revisar normales, orientación de caras, GL_CULL_FACE y estados de OpenGL.
Módulos del proyecto
Módulo	Responsabilidad principal	Código principal
M01	Hoja de Excalibur y geometría facetada	src/objetos/EspadaHoja.cpp
M02	Guarda, mango y pomo de la espada	src/objetos/EspadaEmpunadura.cpp
M03	Piedra irregular, normales y textura	src/objetos/Piedra.cpp
M04	Terreno y elementos cercanos del entorno	src/objetos/Entorno.cpp
M05	Lectura de texturas BMP y coordenadas UV	src/graficos/Texturas.cpp
M06	Cálculo de normales y configuración de culling	src/graficos/Mallas.cpp
M07	Operaciones con cuaterniones y SLERP	src/camara/Cuaterniones.cpp
M08	Vistas y transiciones de cámara	src/camara/Camara.cpp
M09	Iluminación y materiales	src/graficos/Iluminacion.cpp
M10	Controles y coordinación del renderizado	src/controles/Controles.cpp, src/main.cpp
Ampliación: ambiente	Agua, montañas, horizonte y paisaje lejano	src/objetos/Agua.cpp, src/objetos/PaisajeLejano.cpp
Ampliación: interacción	Extracción/devolución de espada y audio	src/objetos/EspadaInteractiva.cpp, src/audio/Audio.cpp


Estructura del repositorio
Excalibur3D/
├── CMakeLists.txt              # Configuración de compilación y dependencias
├── README.md                   # Descripción e instrucciones del proyecto
├── assets/
│   ├── audio/ambiente.wav      # Música ambiental
│   └── texturas/
│       ├── roca.bmp
│       └── suelo.bmp
├── include/excalibur/         # Interfaces y contratos entre módulos
├── src/
│   ├── main.cpp                # Ventana, inicialización y renderizado
│   ├── objetos/                # Espada, piedra, entorno, agua y paisaje
│   ├── graficos/               # Texturas, mallas, iluminación
│   ├── camara/                 # Cuaterniones y cámara
│   ├── controles/              # Eventos de teclado
│   └── audio/                  # Música ambiental
├── docs/                       # Contratos y documentación de módulos
└── pruebas/m01/               # Prueba de la hoja con FreeGLUT simulado
Funcionamiento general
1. src/main.cpp crea la ventana de FreeGLUT y configura la profundidad, la iluminación, las texturas y el descarte de caras posteriores.
2. Cada fotograma aplica la cámara y la iluminación, y después dibuja el entorno, la piedra y la espada, en ese orden.
3. La hoja y la empuñadura comparten una transformación; al pulsar E o R se desplazan juntas sin alterar sus mallas originales.
4. Un temporizador actualiza las transiciones de cámara mediante SLERP y el progreso de la animación de extracción.
5. Controles.cpp recibe las teclas; Audio.cpp activa o detiene la música usando PlaySoundA en Windows.
Los modelos conservan y restauran los estados gráficos cuando corresponde para evitar que las texturas, materiales o transformaciones de un objeto afecten indebidamente a otro.
Organización y colaboración
- development: rama de integración de las funcionalidades.
- master: rama base del repositorio; puede estar desactualizada respecto de development.
- feature/m01-espada-hoja a feature/m10-integracion: ramas históricas de desarrollo de los diez módulos.
- feature/ambiente y feature/interaccion-excalibur: mejoras adicionales que ya se integraron en development.
Para realizar cambios nuevos, se recomienda crear una rama desde development, documentar los archivos afectados, compilar y abrir un Pull Request hacia development. No subir los directorios de compilación (build/, cmake-build-debug/), ejecutables ni DLL de la computadora local.
Pruebas y estado de verificación
El repositorio contiene documentación y evidencias en docs/ y un conjunto de pruebas del M01 en pruebas/m01/, que utiliza un encabezado simulado de FreeGLUT para comprobar aspectos de la malla sin abrir una ventana gráfica.

Estado: los diez módulos y las dos ampliaciones están integrados en development. Esto no significa que todas las pruebas de aceptación estén cerradas. Antes de la entrega final se deben comprobar en el ejecutable las vistas 1/2/3, textura real, diferencia entre normales facetadas y suaves, sentido de las caras con GL_CULL_FACE, transiciones SLERP, controles E/R/M/P/L, redimensionamiento y funcionamiento en otro equipo.

La lista de aceptación está en [`docs/PRUEBAS.md`](docs/PRUEBAS.md). Las comprobaciones generales que siguen sin marcarse allí no deben presentarse como pruebas ya completadas.
Documentación complementaria
- [`docs/CONTRATOS.md`](docs/CONTRATOS.md): interfaces y convenciones de coordenadas.
- [`docs/PRUEBAS.md`](docs/PRUEBAS.md): criterios de aceptación y errores frecuentes.
- [`docs/ambiente/README_AMBIENTE.md`](docs/ambiente/README_AMBIENTE.md): paisaje, agua y capturas.
- [`docs/INTERACCION_EXCALIBUR.md`](docs/INTERACCION_EXCALIBUR.md): extracción de espada y música.
- [`docs/M08_Camara_3D_y_transiciones.md`](docs/M08_Camara_3D_y_transiciones.md): cámara y SLERP.
- [`docs/Documentacion M4/README_M04.md`](docs/Documentacion%20M4/README_M04.md): trabajo del módulo de entorno.
Excalibur3D es un proyecto académico colaborativo. Las bibliotecas externas se utilizan conforme a sus respectivas licencias; la documentación del módulo de audio indica que ambiente.wav fue generado para este proyecto.
