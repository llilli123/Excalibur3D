#pragma once

namespace excalibur::matematica {
    struct Cuaternion { float w{1.0f}, x{}, y{}, z{}; };
    Cuaternion normalizar(Cuaternion q);
    // M07 PENDIENTE: esta funcion es un placeholder, NO implementa SLERP aun.
    Cuaternion slerp(Cuaternion desde, Cuaternion hasta, float t);
}
