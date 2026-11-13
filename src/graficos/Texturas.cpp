// =============================================================================
//  EXCALIBUR 3D · M05 - Texturas y coordenadas UV
// =============================================================================
#include <GL/freeglut.h>
#include "excalibur/Graficos.h"
#include "excalibur/Texturas.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

// Las cabeceras de OpenGL de Windows solo llegan a OpenGL 1.1
#ifndef GL_GENERATE_MIPMAP
#define GL_GENERATE_MIPMAP 0x8191
#endif

// CMakeLists.txt define EXCALIBUR_ASSETS_DIR con la ruta absoluta de assets/
#ifndef EXCALIBUR_ASSETS_DIR
#define EXCALIBUR_ASSETS_DIR "assets"
#endif

namespace excalibur::graficos {

namespace {

struct Imagen {
    int ancho = 0, alto = 0;
    std::vector<unsigned char> rgb;   // filas de ABAJO hacia ARRIBA (como espera OpenGL)
};

GLuint g_ids[static_cast<int>(Textura::Total)] = {0, 0};

unsigned int leer16(const unsigned char* p) {
    return static_cast<unsigned int>(p[0]) | (static_cast<unsigned int>(p[1]) << 8);
}
unsigned int leer32(const unsigned char* p) {
    return static_cast<unsigned int>(p[0]) | (static_cast<unsigned int>(p[1]) << 8) |
           (static_cast<unsigned int>(p[2]) << 16) | (static_cast<unsigned int>(p[3]) << 24);
}

bool cargarBMP(const std::string& ruta, Imagen& img) {
    std::ifstream f(ruta, std::ios::binary);
    if (!f) {
        std::fprintf(stderr, "[M05] No existe: %s\n", ruta.c_str());
        return false;
    }
    std::vector<unsigned char> b((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

    if (b.size() < 54 || b[0] != 'B' || b[1] != 'M') {
        std::fprintf(stderr, "[M05] %s no es un BMP\n", ruta.c_str());
        return false;
    }
    const size_t inicio = leer32(&b[10]);
    const int ancho = static_cast<int>(leer32(&b[18]));
    const int altoArchivo = static_cast<int>(leer32(&b[22]));
    const unsigned int bits = leer16(&b[28]);
    const unsigned int compresion = leer32(&b[30]);

    if ((bits != 24 && bits != 32) || (compresion != 0 && compresion != 3) || ancho <= 0 || altoArchivo == 0) {
        std::fprintf(stderr, "[M05] %s: guarde la imagen como BMP de 24 bits\n", ruta.c_str());
        return false;
    }
    const int alto = std::abs(altoArchivo);
    const size_t bytesFila = ((static_cast<size_t>(bits) * ancho + 31) / 32) * 4;
    if (inicio + bytesFila * alto > b.size()) {
        std::fprintf(stderr, "[M05] %s esta incompleto\n", ruta.c_str());
        return false;
    }

    img.ancho = ancho;
    img.alto = alto;
    img.rgb.resize(static_cast<size_t>(ancho) * alto * 3);
    const size_t bpp = bits / 8;
    for (int fila = 0; fila < alto; ++fila) {
        const int filaArchivo = (altoArchivo < 0) ? (alto - 1 - fila) : fila;
        const unsigned char* src = &b[inicio + bytesFila * filaArchivo];
        unsigned char* dst = &img.rgb[static_cast<size_t>(fila) * ancho * 3];
        for (int x = 0; x < ancho; ++x) {
            dst[x * 3 + 0] = src[x * bpp + 2];   // R
            dst[x * 3 + 1] = src[x * bpp + 1];   // G
            dst[x * 3 + 2] = src[x * bpp + 0];   // B
        }
    }
    return true;
}

// Si falta la imagen, tablero gris para que el objeto no salga blanco
Imagen tableroReemplazo() {
    Imagen img;
    img.ancho = img.alto = 64;
    img.rgb.resize(64 * 64 * 3);
    for (int y = 0; y < 64; ++y) {
        for (int x = 0; x < 64; ++x) {
            const unsigned char c = (((x / 8) + (y / 8)) % 2) ? 90 : 200;
            unsigned char* p = &img.rgb[static_cast<size_t>(y * 64 + x) * 3];
            p[0] = p[1] = p[2] = c;
        }
    }
    return img;
}

// OpenGL 1.4 o mayor -> mipmaps automaticos disponibles
bool soportaMipmaps() {
    const char* v = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    if (!v) return false;
    char* fin = nullptr;
    const long mayor = std::strtol(v, &fin, 10);
    const long menor = (fin && *fin == '.') ? std::strtol(fin + 1, nullptr, 10) : 0;
    return mayor > 1 || (mayor == 1 && menor >= 4);
}

// Sube la imagen a la tarjeta de video y devuelve su ID
GLuint crearTextura(const Imagen& img) {
    const bool mipmaps = soportaMipmaps();
    GLuint id = 0;
    glGenTextures(1, &id);                    // 1. pedir un ID de textura
    glBindTexture(GL_TEXTURE_2D, id);         // 2. activarla para configurarla
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);    //    las filas RGB no traen relleno

    // 3. UV fuera de 0..1 -> repetir la imagen (asi el suelo la repite varias veces)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   // eje u
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);   // eje v

    // 4. Filtrado: de lejos (MIN) con mipmaps, de cerca (MAG) lineal
    if (mipmaps) glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 5. Copiar los pixeles a la memoria de video
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.ancho, img.alto, 0, GL_RGB, GL_UNSIGNED_BYTE, img.rgb.data());

    glBindTexture(GL_TEXTURE_2D, 0);
    return id;
}

// Busca la imagen: primero en la ruta que fija CMake, luego relativa al .exe
std::string rutaImagen(const std::string& nombre) {
    const std::string candidatas[] = {
        std::string(EXCALIBUR_ASSETS_DIR) + "/texturas/" + nombre,
        "assets/texturas/" + nombre,
        "../assets/texturas/" + nombre,
        "../../assets/texturas/" + nombre,
        "../../../assets/texturas/" + nombre,
    };
    for (const std::string& c : candidatas) {
        std::ifstream f(c);
        if (f.good()) return c;
    }
    return candidatas[0];
}

void cargar(Textura t, const char* archivo) {
    Imagen img;
    const bool ok = cargarBMP(rutaImagen(archivo), img);
    if (!ok) img = tableroReemplazo();
    g_ids[static_cast<int>(t)] = crearTextura(img);
    if (ok) std::printf("[M05] Textura cargada: %s (%dx%d)\n", archivo, img.ancho, img.alto);
}

} // namespace

// =============================================================================
// Declaradas en Graficos.h (main.cpp ya las llama)
// =============================================================================
void inicializarTexturas() {
    liberarTexturas();
    cargar(Textura::Roca, "roca.bmp");
    cargar(Textura::Suelo, "suelo.bmp");
    // Color final = color de la foto x iluminacion (respeta la luz de M09)
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void liberarTexturas() {
    for (GLuint& id : g_ids) {
        if (id != 0) glDeleteTextures(1, &id);
        id = 0;
    }
}

// =============================================================================
// Declaradas en Texturas.h (para M03 y M04)
// =============================================================================
void activarTextura(Textura t) {
    if (t == Textura::Total || g_ids[static_cast<int>(t)] == 0) return;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_ids[static_cast<int>(t)]);
    glColor3f(1.0f, 1.0f, 1.0f);   // blanco: con GL_COLOR_MATERIAL no oscurece la foto
}

void desactivarTextura() {
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

// u,v = posicion x,z del vertice llevada a 0..1 y multiplicada por las repeticiones.
// Suelo de 12x12 con 4 repeticiones -> la foto se repite 4x4 (cada copia mide 3x3).
Vec2 uvPlanarXZ(float x, float z, float tamanoLado, float repeticiones) {
    if (tamanoLado <= 0.0f) tamanoLado = 1.0f;
    return { (x / tamanoLado + 0.5f) * repeticiones,
             (0.5f - z / tamanoLado) * repeticiones };
}

// Como un globo terraqueo: u = longitud (vuelta alrededor de Y), v = latitud.
Vec2 uvEsferica(float dx, float dy, float dz) {
    const float PI = 3.14159265f;
    const float l = std::sqrt(dx * dx + dy * dy + dz * dz);
    if (l < 1e-6f) return {0.5f, 0.5f};
    float ny = dy / l;
    if (ny > 1.0f) ny = 1.0f;
    if (ny < -1.0f) ny = -1.0f;
    return { 0.5f + std::atan2(dz / l, dx / l) / (2.0f * PI),
             0.5f + std::asin(ny) / PI };
}

// Si un triangulo cruza la costura (u = 0.98 y u = 0.02), se suma 1 a las u
// pequenas; con GL_REPEAT, 1.02 se ve igual que 0.02 y no se estira la foto.
void corregirCostura(Vec2& a, Vec2& b, Vec2& c) {
    const float minU = std::fmin(a.u, std::fmin(b.u, c.u));
    const float maxU = std::fmax(a.u, std::fmax(b.u, c.u));
    if (maxU - minU > 0.5f) {
        if (a.u < 0.5f) a.u += 1.0f;
        if (b.u < 0.5f) b.u += 1.0f;
        if (c.u < 0.5f) c.u += 1.0f;
    }
}

} // namespace excalibur::graficos
