
cmake_minimum_required(VERSION 3.16)

# Compatibilidad de dependencias antiguas con CMake 4.x
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(Excalibur3D LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================
# 1. OPENGL
# ============================================================

find_package(OpenGL REQUIRED)

# ============================================================
# 2. FREEGLUT
# ============================================================

if(WIN32)

    # Compilar FreeGLUT estaticamente para evitar depender
    # de libfreeglut.dll durante la ejecucion.

    include(FetchContent)

    set(FREEGLUT_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
    set(FREEGLUT_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(FREEGLUT_BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)

    FetchContent_Declare(
            freeglut
            GIT_REPOSITORY https://github.com/freeglut/freeglut.git
            GIT_TAG v3.4.0
    )

    FetchContent_MakeAvailable(freeglut)

    if(NOT TARGET freeglut_static)
        message(FATAL_ERROR "No se pudo compilar FreeGLUT estatico")
    endif()

else()

    # Otros sistemas: usar la biblioteca instalada.
    find_package(GLUT REQUIRED)

endif()

# ============================================================
# 3. ARCHIVOS DEL PROYECTO
# ============================================================

add_executable(Excalibur3D

        src/main.cpp

        # Objetos
        src/objetos/EspadaHoja.cpp
        src/objetos/EspadaEmpunadura.cpp
        src/objetos/Piedra.cpp
        src/objetos/Entorno.cpp
        src/objetos/PaisajeLejano.cpp
        src/objetos/Agua.cpp
        src/objetos/EspadaInteractiva.cpp

        # Graficos
        src/graficos/Texturas.cpp
        src/graficos/Mallas.cpp
        src/graficos/Iluminacion.cpp

        # Camara
        src/camara/Cuaterniones.cpp
        src/camara/Camara.cpp

        # Controles
        src/controles/Controles.cpp

        # Audio
        src/audio/Audio.cpp
)

target_include_directories(Excalibur3D PRIVATE include)

# ============================================================
# 4. ENLAZAR BIBLIOTECAS
# ============================================================

if(WIN32)

    target_link_libraries(Excalibur3D PRIVATE
            freeglut_static
            OpenGL::GL
            OpenGL::GLU
            winmm
    )

    # Cabeceras de FreeGLUT descargado (GL/freeglut.h)
    target_include_directories(Excalibur3D PRIVATE
            ${freeglut_SOURCE_DIR}/include
    )

else()

    target_link_libraries(Excalibur3D PRIVATE
            OpenGL::GL
            OpenGL::GLU
            GLUT::GLUT
    )

    target_include_directories(Excalibur3D PRIVATE
            ${GLUT_INCLUDE_DIRS}
    )

endif()

# ============================================================
# 5. OPCIONES DEL COMPILADOR
# ============================================================

if(MSVC)

    target_compile_options(Excalibur3D PRIVATE /W4 /EHsc)

else()

    target_compile_options(Excalibur3D PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )

endif()

# ============================================================
# 6. RUTA DE TEXTURAS
# ============================================================

target_compile_definitions(Excalibur3D PRIVATE
        EXCALIBUR_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
)

# ============================================================
# 7. DLL DE MINGW EN WINDOWS
# ============================================================

if(WIN32 AND MINGW)

    # Detectar el directorio bin del compilador configurado.
    get_filename_component(
            MINGW_BIN
            "${CMAKE_CXX_COMPILER}"
            DIRECTORY
    )

    set(DLLS_MINGW
            libgcc_s_seh-1.dll
            libstdc++-6.dll
            libwinpthread-1.dll
    )

    foreach(DLL IN LISTS DLLS_MINGW)

        if(EXISTS "${MINGW_BIN}/${DLL}")

            add_custom_command(
                    TARGET Excalibur3D POST_BUILD

                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${MINGW_BIN}/${DLL}"
                    "$<TARGET_FILE_DIR:Excalibur3D>"

                    COMMENT "Copiando ${DLL}"
                    VERBATIM
            )

        else()

            message(STATUS
                    "DLL opcional no encontrada: ${MINGW_BIN}/${DLL}"
            )

        endif()

    endforeach()

endif()
