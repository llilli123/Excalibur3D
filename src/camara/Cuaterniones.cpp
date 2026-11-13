#include <algorithm>
#include <cmath>
#include "excalibur/Cuaterniones.h"

namespace excalibur::matematica {


    namespace {
        float productoEscalar(Cuaternion a, Cuaternion b) {
            return a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
        }
    }

Cuaternion normalizar(Cuaternion q) {
    const float longitud = std::sqrt(q.w*q.w+q.x*q.x+q.y*q.y+q.z*q.z);
    if (longitud < 1e-7f) return {};
    return {q.w/longitud,q.x/longitud,q.y/longitud,q.z/longitud};
}
Cuaternion slerp(Cuaternion desde, Cuaternion hasta, float t) {
    desde = normalizar(desde);
    hasta = normalizar(hasta);
    t = std::clamp(t, 0.0f, 1.0f);

    float cosAngulo = productoEscalar(desde, hasta);
    if (cosAngulo < 0.0f) {
        // q y -q representan el mismo giro. Elegimos el arco corto.
        hasta = { -hasta.w, -hasta.x, -hasta.y, -hasta.z };
        cosAngulo = -cosAngulo;
    }

    // Evita que el redondeo deje el producto escalar fuera del dominio de acos.
    cosAngulo = std::clamp(cosAngulo, 0.0f, 1.0f);

    if (cosAngulo > 0.9995f) {
        // Con orientaciones casi iguales, sin(angulo) se acerca a cero.
        return normalizar({
            desde.w + t * (hasta.w - desde.w),
            desde.x + t * (hasta.x - desde.x),
            desde.y + t * (hasta.y - desde.y),
            desde.z + t * (hasta.z - desde.z)
            });
    }

    const float angulo = std::acos(cosAngulo);
    const float senoAngulo = std::sin(angulo);
    const float pesoDesde = std::sin((1.0f - t) * angulo) / senoAngulo;
    const float pesoHasta = std::sin(t * angulo) / senoAngulo;

    return normalizar({
        pesoDesde * desde.w + pesoHasta * hasta.w,
        pesoDesde * desde.x + pesoHasta * hasta.x,
        pesoDesde * desde.y + pesoHasta * hasta.y,
        pesoDesde * desde.z + pesoHasta * hasta.z
        });
}

}
