#include <GL/freeglut.h>
#include <cmath>
#include "excalibur/Graficos.h"

namespace excalibur::graficos {
namespace {
    Vec3 restar(Vec3 a, Vec3 b) { return {a.x-b.x,a.y-b.y,a.z-b.z}; }
    Vec3 cruz(Vec3 a, Vec3 b) {
        return {a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x};
    }
    Vec3 normalizarVec(Vec3 n) {
        const float l = std::sqrt(n.x*n.x+n.y*n.y+n.z*n.z);
        if (l <= 1e-7f) return {0.0f,1.0f,0.0f};
        return {n.x/l,n.y/l,n.z/l};
    }
}
Vec3 normalDeCara(Vec3 a, Vec3 b, Vec3 c) {
    return normalizarVec(cruz(restar(b,a),restar(c,a)));
}
std::vector<Vec3> normalesSuaves(const std::vector<Vec3>& posiciones,
                                 const std::vector<unsigned>& indices) {
    std::vector<Vec3> normales(posiciones.size(),{0,0,0});
    for (std::size_t i=0; i+2<indices.size(); i+=3) {
        const unsigned a=indices[i], b=indices[i+1], c=indices[i+2];
        if(a>=posiciones.size() || b>=posiciones.size() || c>=posiciones.size()) continue;
        const Vec3 ab=restar(posiciones[b],posiciones[a]);
        const Vec3 ac=restar(posiciones[c],posiciones[a]);
        const Vec3 peso=cruz(ab,ac); // pondera por area de triangulo
        for (unsigned v : {a,b,c}) {
            normales[v].x += peso.x;
            normales[v].y += peso.y;
            normales[v].z += peso.z;
        }
    }
    for (Vec3& n: normales) n=normalizarVec(n);
    return normales;
}
void configurarCulling() {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    // M06: verificar winding de las caras de M01-M04 y explicar
    // diferencia entre normales por cara y por vertice sobre mallas propias.
}
}
