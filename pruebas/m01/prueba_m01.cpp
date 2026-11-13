// Pruebas de aceptacion de M01 sobre el codigo REAL de src/objetos/EspadaHoja.cpp
// Usa un FreeGLUT simulado (stub/GL/freeglut.h) que registra las llamadas a OpenGL.
#include <GL/freeglut.h>
#include <cmath>
#include <cstdio>
#include <map>
#include <string>
#include "excalibur/Objetos.h"

static int fallos = 0;
#define CHECK(cond, msg) do { if (cond) std::printf("  [OK]    %s\n", msg); else { std::printf("  [FALLA] %s\n", msg); ++fallos; } } while (0)
static std::string clave(const float* p) { char b[96]; std::snprintf(b, sizeof b, "%.4f,%.4f,%.4f", p[0], p[1], p[2]); return b; }

int main() {
    std::printf("== M01 Hoja de Excalibur: pruebas ==\n");
    excalibur::objetos::dibujarHojaEspada();
    auto& r = gl();

    std::printf("Estado OpenGL\n");
    CHECK(r.profundidad == 0, "glPushMatrix/glPopMatrix balanceados");
    CHECK(r.attrib == 0 && r.maxAttrib >= 1, "glPushAttrib/glPopAttrib balanceados: luz, mezcla y profundidad se restauran");
    CHECK(r.begin == r.end && !r.dentroBegin, "glBegin/glEnd balanceados");
    CHECK(!r.modosBloque.empty() && r.modosBloque[0] == GL_TRIANGLES, "el primer bloque es la malla solida (GL_TRIANGLES)");

    // La malla solida es el bloque 0; los demas bloques son efectos.
    std::vector<RegistroGL::V> V;
    for (const auto& v : r.vertices) if (v.bloque == 0) V.push_back(v);
    CHECK(!V.empty() && V.size() % 3 == 0, "vertices de la malla multiplo de 3");
    const std::size_t nTri = V.size() / 3;
    std::printf("  info: malla %zu triangulos; %zu bloques de efectos\n", nTri, r.modosBloque.size() - 1);

    std::printf("Normales por cara y winding CCW\n");
    int distintas = 0, noUnit = 0, noCCW = 0;
    double volumen = 0;
    std::map<std::string, int> aristas;
    float minY = 1e9f, maxY = -1e9f, maxX = 0, maxZ = 0, minU = 1e9f, maxU = -1e9f, minV = 1e9f, maxV = -1e9f;
    for (std::size_t i = 0; i < nTri; ++i) {
        const auto& a = V[3*i]; const auto& b = V[3*i+1]; const auto& c = V[3*i+2];
        float ab[3], ac[3], n[3];
        for (int k = 0; k < 3; ++k) { ab[k] = b.p[k]-a.p[k]; ac[k] = c.p[k]-a.p[k]; }
        n[0] = ab[1]*ac[2]-ab[2]*ac[1]; n[1] = ab[2]*ac[0]-ab[0]*ac[2]; n[2] = ab[0]*ac[1]-ab[1]*ac[0];
        // Volumen con signo (teorema de la divergencia): > 0 si todas las caras miran hacia fuera.
        volumen += (a.p[0]*(b.p[1]*c.p[2]-b.p[2]*c.p[1]) - a.p[1]*(b.p[0]*c.p[2]-b.p[2]*c.p[0])
                  + a.p[2]*(b.p[0]*c.p[1]-b.p[1]*c.p[0])) / 6.0;
        const float l = std::sqrt(n[0]*n[0]+n[1]*n[1]+n[2]*n[2]);
        for (const auto* v : {&a,&b,&c})
            if (std::fabs(v->n[0]-a.n[0]) + std::fabs(v->n[1]-a.n[1]) + std::fabs(v->n[2]-a.n[2]) > 1e-6f) ++distintas;
        if (std::fabs(std::sqrt(a.n[0]*a.n[0]+a.n[1]*a.n[1]+a.n[2]*a.n[2]) - 1.0f) > 1e-4f) ++noUnit;
        if (l > 1e-9f && (n[0]*a.n[0]+n[1]*a.n[1]+n[2]*a.n[2]) / l < 0.999f) ++noCCW;
        for (const auto* p : {&a,&b,&c}) {
            minY = std::fmin(minY, p->p[1]); maxY = std::fmax(maxY, p->p[1]);
            maxX = std::fmax(maxX, std::fabs(p->p[0])); maxZ = std::fmax(maxZ, std::fabs(p->p[2]));
            minU = std::fmin(minU, p->t[0]); maxU = std::fmax(maxU, p->t[0]);
            minV = std::fmin(minV, p->t[1]); maxV = std::fmax(maxV, p->t[1]);
        }
        const float* t3[3] = {a.p, b.p, c.p};
        for (int e = 0; e < 3; ++e) aristas[clave(t3[e]) + ">" + clave(t3[(e+1)%3])]++;
    }
    CHECK(distintas == 0, "cada triangulo usa UNA sola normal (sombreado facetado)");
    CHECK(noUnit == 0, "normales unitarias");
    CHECK(noCCW == 0, "normal enviada == (b-a)x(c-a): winding CCW coherente");
    std::printf("  info: volumen con signo = %.5f\n", volumen);
    CHECK(volumen > 0, "volumen positivo: las caras miran hacia fuera (GL_CULL_FACE no borra la hoja)");
    int abiertas = 0, dup = 0;
    for (auto& [k, cnt] : aristas) {
        const auto s = k.find('>');
        const std::string inv = k.substr(s + 1) + ">" + k.substr(0, s);
        if (!aristas.count(inv)) ++abiertas;
        if (cnt > 1) ++dup;
    }
    CHECK(abiertas == 0 && dup == 0, "malla cerrada y orientada: cada arista en 2 caras opuestas (punta cerrada)");

    std::printf("Medidas (docs/CONTRATOS.md)\n");
    std::printf("  info: y=[%.3f, %.3f]  |x|<=%.3f  |z|<=%.3f\n", minY, maxY, maxX, maxZ);
    CHECK(std::fabs(minY - 1.17f) < 0.02f, "punta en y~1.17 (dentro de la piedra, tope ~1.41)");
    CHECK(maxY > 2.835f && maxY < 2.90f, "base metida en la guarda de M02 (cara inferior 2.835)");
    CHECK(maxX <= 0.49f && maxZ <= 0.10f, "hoja mas angosta que la guarda (0.98 x 0.20)");

    std::printf("UV para M05\n");
    CHECK(minU >= -1e-4f && maxU <= 1.0001f && minV >= -1e-4f && maxV <= 1.0001f, "UV dentro de [0,1]");

    std::printf("Efectos\n");
    int lineas = 0, puntos = 0;
    for (auto m : r.modosBloque) { if (m == GL_LINES || m == GL_LINE_STRIP) ++lineas; if (m == GL_POINTS) ++puntos; }
    CHECK(lineas >= 3 && puntos >= 1, "runas, brillo de filos y chispas presentes");

    std::printf("Segunda llamada\n");
    const std::size_t antes = r.vertices.size();
    excalibur::objetos::dibujarHojaEspada();
    CHECK(gl().vertices.size() == 2 * antes, "cada frame dibuja lo mismo (malla cacheada)");

    std::printf("== %s (%d fallos) ==\n", fallos ? "HAY FALLOS" : "TODO OK", fallos);
    return fallos ? 1 : 0;
}
