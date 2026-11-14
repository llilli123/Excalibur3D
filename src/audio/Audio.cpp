#include "excalibur/Audio.h"
#include <iostream>
#include <string>

#ifdef _WIN32
    #include <windows.h>
    #include <mmsystem.h>
    #ifdef _MSC_VER
        #pragma comment(lib, "winmm.lib")
    #endif
#endif

#ifndef EXCALIBUR_ASSETS_DIR
#define EXCALIBUR_ASSETS_DIR "assets"
#endif

namespace excalibur::audio {
namespace {
    bool sonando = false;
    const std::string RUTA_MUSICA = std::string(EXCALIBUR_ASSETS_DIR) + "/audio/ambiente.wav";
}

void alternarMusica() {
#ifdef _WIN32
    if (!sonando) {
        // SND_ASYNC: no bloquea el programa. SND_LOOP: se repite en bucle.
        if (PlaySoundA(RUTA_MUSICA.c_str(), nullptr, SND_FILENAME | SND_ASYNC | SND_LOOP)) {
            sonando = true;
        } else {
            std::cerr << "No se pudo reproducir: " << RUTA_MUSICA << "\n";
        }
    } else {
        detenerMusica();
    }
#else
    // En Linux/macOS el proyecto compila igual, pero sin sonido.
    sonando = !sonando;
    std::cout << "Musica " << (sonando ? "activada" : "desactivada")
              << " (audio solo disponible en Windows)\n";
#endif
}

void detenerMusica() {
#ifdef _WIN32
    PlaySoundA(nullptr, nullptr, 0);
#endif
    sonando = false;
}
}
