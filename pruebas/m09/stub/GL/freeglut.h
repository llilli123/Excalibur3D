// FreeGLUT SIMULADO para pruebas de M09: registra llamadas de iluminacion y materiales.
#pragma once
#include <vector>
#include <cmath>
#include <map>

typedef unsigned int GLenum;
typedef float GLfloat;
typedef int GLint;
typedef unsigned int GLbitfield;
typedef unsigned char GLboolean;

#define GL_FALSE 0
#define GL_TRUE 1

// Enables
#define GL_LIGHTING       0x0B50
#define GL_LIGHT0         0x4000
#define GL_LIGHT1         0x4001
#define GL_NORMALIZE      0x0BA1
#define GL_SMOOTH         0x1D01

// Light parameters
#define GL_AMBIENT        0x1200
#define GL_DIFFUSE        0x1201
#define GL_SPECULAR       0x1202
#define GL_POSITION       0x1203
#define GL_CONSTANT_ATTENUATION  0x1207
#define GL_LINEAR_ATTENUATION    0x1208
#define GL_QUADRATIC_ATTENUATION 0x1209

// Light model
#define GL_LIGHT_MODEL_AMBIENT      0x0B53
#define GL_LIGHT_MODEL_LOCAL_VIEWER 0x0B51

// Material
#define GL_EMISSION       0x1600
#define GL_SHININESS      0x1601
#define GL_FRONT_AND_BACK 0x0408
#define GL_FRONT          0x0404

struct RegistroIluminacionGL {
    bool lightingHabilitado = false;
    bool normalizeHabilitado = false;
    bool light0Habilitado = false;
    bool light1Habilitado = false;
    GLenum shadeModel = 0;

    int localViewer = 0;
    float globalAmbient[4] = {0,0,0,0};

    struct LuzInfo {
        bool habilitada = false;
        float ambiente[4] = {0,0,0,0};
        float difusa[4] = {0,0,0,0};
        float especular[4] = {0,0,0,0};
        float posicion[4] = {0,0,0,0};
        float atenuacionConst = 1.0f;
        float atenuacionLin = 0.0f;
        float atenuacionCuad = 0.0f;
        int conteoSetPosicion = 0;
    };
    LuzInfo luz0;
    LuzInfo luz1;

    struct MaterialInfo {
        float especular[4] = {0,0,0,0};
        float emision[4] = {0,0,0,0};
        float shininess = 0.0f;
        float color[3] = {0,0,0};
    } materialActual;

    std::vector<MaterialInfo> historialMateriales;
};

inline RegistroIluminacionGL& glReg() {
    static RegistroIluminacionGL r;
    return r;
}

inline void glEnable(GLenum cap) {
    auto& r = glReg();
    if (cap == GL_LIGHTING)  r.lightingHabilitado = true;
    if (cap == GL_NORMALIZE) r.normalizeHabilitado = true;
    if (cap == GL_LIGHT0)    { r.light0Habilitado = true; r.luz0.habilitada = true; }
    if (cap == GL_LIGHT1)    { r.light1Habilitado = true; r.luz1.habilitada = true; }
}

inline void glDisable(GLenum cap) {
    auto& r = glReg();
    if (cap == GL_LIGHTING)  r.lightingHabilitado = false;
    if (cap == GL_NORMALIZE) r.normalizeHabilitado = false;
    if (cap == GL_LIGHT0)    { r.light0Habilitado = false; r.luz0.habilitada = false; }
    if (cap == GL_LIGHT1)    { r.light1Habilitado = false; r.luz1.habilitada = false; }
}

inline void glShadeModel(GLenum mode) {
    glReg().shadeModel = mode;
}

inline void glLightModelfv(GLenum pname, const GLfloat* params) {
    if (pname == GL_LIGHT_MODEL_AMBIENT) {
        for (int i = 0; i < 4; ++i) glReg().globalAmbient[i] = params[i];
    }
}

inline void glLightModeli(GLenum pname, GLint param) {
    if (pname == GL_LIGHT_MODEL_LOCAL_VIEWER) {
        glReg().localViewer = param;
    }
}

inline void glLightfv(GLenum light, GLenum pname, const GLfloat* params) {
    auto& r = glReg();
    RegistroIluminacionGL::LuzInfo* l = nullptr;
    if (light == GL_LIGHT0) l = &r.luz0;
    else if (light == GL_LIGHT1) l = &r.luz1;
    if (!l) return;

    if (pname == GL_AMBIENT)  for (int i = 0; i < 4; ++i) l->ambiente[i] = params[i];
    if (pname == GL_DIFFUSE)  for (int i = 0; i < 4; ++i) l->difusa[i] = params[i];
    if (pname == GL_SPECULAR) for (int i = 0; i < 4; ++i) l->especular[i] = params[i];
    if (pname == GL_POSITION) {
        for (int i = 0; i < 4; ++i) l->posicion[i] = params[i];
        l->conteoSetPosicion++;
    }
}

inline void glLightf(GLenum light, GLenum pname, GLfloat param) {
    auto& r = glReg();
    RegistroIluminacionGL::LuzInfo* l = nullptr;
    if (light == GL_LIGHT0) l = &r.luz0;
    else if (light == GL_LIGHT1) l = &r.luz1;
    if (!l) return;

    if (pname == GL_CONSTANT_ATTENUATION)  l->atenuacionConst = param;
    if (pname == GL_LINEAR_ATTENUATION)    l->atenuacionLin = param;
    if (pname == GL_QUADRATIC_ATTENUATION) l->atenuacionCuad = param;
}

inline void glMaterialfv(GLenum, GLenum pname, const GLfloat* params) {
    auto& r = glReg();
    if (pname == GL_SPECULAR) for (int i = 0; i < 4; ++i) r.materialActual.especular[i] = params[i];
    if (pname == GL_EMISSION) for (int i = 0; i < 4; ++i) r.materialActual.emision[i] = params[i];
    r.historialMateriales.push_back(r.materialActual);
}

inline void glMaterialf(GLenum, GLenum pname, GLfloat param) {
    auto& r = glReg();
    if (pname == GL_SHININESS) r.materialActual.shininess = param;
}

inline void glColor3f(GLfloat red, GLfloat green, GLfloat blue) {
    auto& r = glReg();
    r.materialActual.color[0] = red;
    r.materialActual.color[1] = green;
    r.materialActual.color[2] = blue;
}
