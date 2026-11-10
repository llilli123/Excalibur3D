#include <cmath>
#include "excalibur/Cuaterniones.h"

namespace excalibur::matematica {
Cuaternion normalizar(Cuaternion q) {
    const float longitud = std::sqrt(q.w*q.w+q.x*q.x+q.y*q.y+q.z*q.z);
    if (longitud < 1e-7f) return {};
    return {q.w/longitud,q.x/longitud,q.y/longitud,q.z/longitud};
}
Cuaternion slerp(Cuaternion desde, Cuaternion hasta, float t) {
    // M07 PENDIENTE: Implementar SLERP real (producto punto, signo,
    // angulo, seno; fallback para angulos muy pequenos). Este valor
    // NO INTERPOLA: solo mantiene la orientacion de origen.
    (void)hasta;
    (void)t;
    return normalizar(desde);
}
}
