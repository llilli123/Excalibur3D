cmake_minimum_required(VERSION 3.16)
project(Excalibur3D LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(Excalibur3D
    src/main.cpp
    src/objetos/EspadaHoja.cpp
    src/objetos/EspadaEmpunadura.cpp
    src/objetos/Piedra.cpp
    src/modulos/m04_entorno/Entorno.cpp
    src/graficos/Texturas.cpp
    src/graficos/Mallas.cpp
    src/graficos/Iluminacion.cpp
    src/camara/Cuaterniones.cpp
    src/camara/Camara.cpp
    src/controles/Controles.cpp
)

target_include_directories(Excalibur3D PRIVATE include ${GLUT_INCLUDE_DIRS})
# OpenGL::GLU exists in standard CMake FindOpenGL on Windows and Linux.
target_link_libraries(Excalibur3D PRIVATE OpenGL::GL OpenGL::GLU GLUT::GLUT)

if(MSVC)
    target_compile_options(Excalibur3D PRIVATE /W4)
else()
    target_compile_options(Excalibur3D PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Textures should be located using a stable path if work directory differs.
target_compile_definitions(Excalibur3D PRIVATE EXCALIBUR_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets")
