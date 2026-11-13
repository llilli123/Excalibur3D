// FreeGLUT SIMULADO para pruebas: registra las llamadas en vez de dibujar.
#pragma once
#include <vector>
typedef unsigned int GLenum; typedef float GLfloat; typedef unsigned int GLbitfield; typedef unsigned char GLboolean;
#define GL_POINTS 0
#define GL_LINES 1
#define GL_LINE_STRIP 3
#define GL_TRIANGLES 4
#define GL_FALSE 0
#define GL_TRUE 1
#define GL_CURRENT_BIT 0x1
#define GL_POINT_BIT 0x2
#define GL_LINE_BIT 0x4
#define GL_LIGHTING_BIT 0x40
#define GL_DEPTH_BUFFER_BIT 0x100
#define GL_ENABLE_BIT 0x2000
#define GL_COLOR_BUFFER_BIT 0x4000
#define GL_LIGHTING 0x0B50
#define GL_BLEND 0x0BE2
#define GL_LINE_SMOOTH 0x0B20
#define GL_POINT_SMOOTH 0x0B10
#define GL_SRC_ALPHA 0x0302
#define GL_ONE 1
#define GL_FRONT 0x0404
#define GL_SPECULAR 0x1202
#define GL_SHININESS 0x1601
#define GL_CULL_FACE 0x0B44
#define GL_BACK 0x0405
#define GL_CCW 0x0901
#define GLUT_ELAPSED_TIME 700
struct RegistroGL {
    int profundidad = 0, attrib = 0, maxAttrib = 0, begin = 0, end = 0, bloque = -1;
    bool dentroBegin = false; GLenum modo = 0;
    float n[3] = {0,0,0}, t[2] = {0,0}, c[3] = {0,0,0};
    struct V { float p[3], n[3], t[2], c[3]; GLenum modo; int bloque; };
    std::vector<V> vertices;
    std::vector<GLenum> modosBloque;
};
inline RegistroGL& gl() { static RegistroGL r; return r; }
inline void glPushMatrix() { gl().profundidad++; }
inline void glPopMatrix() { gl().profundidad--; }
inline void glPushAttrib(GLbitfield) { auto& r = gl(); r.attrib++; if (r.attrib > r.maxAttrib) r.maxAttrib = r.attrib; }
inline void glPopAttrib() { gl().attrib--; }
inline void glColor3f(float a, float b, float c) { gl().c[0]=a; gl().c[1]=b; gl().c[2]=c; }
inline void glColor4f(float a, float b, float c, float) { glColor3f(a, b, c); }
inline void glBegin(GLenum m) { auto& r = gl(); r.begin++; r.dentroBegin = true; r.modo = m; r.bloque++; r.modosBloque.push_back(m); }
inline void glEnd() { gl().end++; gl().dentroBegin = false; }
inline void glNormal3f(float x, float y, float z) { gl().n[0]=x; gl().n[1]=y; gl().n[2]=z; }
inline void glTexCoord2f(float u, float v) { gl().t[0]=u; gl().t[1]=v; }
inline void glVertex3f(float x, float y, float z) {
    auto& r = gl();
    RegistroGL::V v{{x,y,z},{r.n[0],r.n[1],r.n[2]},{r.t[0],r.t[1]},{r.c[0],r.c[1],r.c[2]},r.modo,r.bloque};
    r.vertices.push_back(v); }
inline void glDisable(GLenum) {} inline void glEnable(GLenum) {}
inline void glBlendFunc(GLenum, GLenum) {} inline void glDepthMask(GLboolean) {}
inline void glLineWidth(float) {} inline void glPointSize(float) {}
inline void glScalef(float, float, float) {}
inline void glMaterialfv(GLenum, GLenum, const float*) {} inline void glMaterialf(GLenum, GLenum, float) {}
inline void glCullFace(GLenum) {} inline void glFrontFace(GLenum) {}
inline int glutGet(GLenum) { return 1234; }
#define GL_LINE_LOOP 2
#define GL_TRIANGLE_FAN 6
#define GL_LESS 0x0201
#define GL_LEQUAL 0x0203
#define GL_MODELVIEW_MATRIX 0x0BA6
inline void glDepthFunc(GLenum) {}
inline void glGetFloatv(GLenum, float* m) { // camara simulada en (0,2,5) mirando -Z
    for (int i = 0; i < 16; ++i) m[i] = (i % 5 == 0) ? 1.0f : 0.0f;
    m[13] = -2.0f; m[14] = -5.0f; }
