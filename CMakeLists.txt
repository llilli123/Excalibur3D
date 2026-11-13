cmake_minimum_required(VERSION 3.16)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)
project(Excalibur3D LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)
find_package(GLUT)

if(NOT GLUT_FOUND)
    message(STATUS "GLUT no encontrado en el sistema. Descargando y compilando FreeGLUT automaticamente con FetchContent...")
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
    elseif(TARGET freeglut AND NOT TARGET GLUT::GLUT)
        add_library(GLUT::GLUT ALIAS freeglut)
    endif()
    set(GLUT_INCLUDE_DIRS ${freeglut_SOURCE_DIR}/include)
endif()

add_executable(Excalibur3D
    src/main.cpp
    src/objetos/EspadaHoja.cpp
    src/objetos/EspadaEmpunadura.cpp
    src/objetos/Piedra.cpp
    src/objetos/Entorno.cpp
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
    target_compile_options(Excalibur3D PRIVATE /W4 /EHsc /wd4505)
else()
    target_compile_options(Excalibur3D PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Textures should be located using a stable path if work directory differs.
target_compile_definitions(Excalibur3D PRIVATE EXCALIBUR_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets")
