#ifndef _EGL_INIT_H
#define _EGL_INIT_H

#include <GLES2/gl2.h>
#include <EGL/egl.h>

bool createEGLDisplay(EGLDisplay&);
bool chooseEGLConfig(EGLDisplay, EGLConfig&);
bool createEGLSurface(EGLDisplay, EGLConfig, EGLSurface&);
bool setupEGLContext(EGLDisplay, EGLConfig, EGLSurface, EGLContext&);

bool egl_init(EGLDisplay&, EGLConfig&, EGLSurface&, EGLContext&);
void egl_cleanup(EGLDisplay& display);

bool testEGLError(const char* last_called);
bool testGLError(const char* last_called);

#endif
