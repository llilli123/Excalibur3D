#pragma once

namespace excalibur::matematica {
    struct Cuaternion { float w{1.0f}, x{}, y{}, z{}; };
    Cuaternion normalizar(Cuaternion q);
    // Interpola por el arco corto con t limitado al intervalo [0, 1].
    Cuaternion slerp(Cuaternion desde, Cuaternion hasta, float t);
}
