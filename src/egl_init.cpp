#include "egl_init.hpp"

#include <cstdio>

void egl_cleanup(EGLDisplay& display) {
	eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglTerminate(display);
}

bool egl_init(EGLDisplay& display, EGLConfig& config, EGLSurface& surface, EGLContext& context) {
    return createEGLDisplay(display) &&
        chooseEGLConfig(display, config) &&
        createEGLSurface(display, config, surface) &&
        setupEGLContext(display, config, surface, context);
}

bool createEGLDisplay(EGLDisplay& display) {

	display = eglGetDisplay((EGLNativeDisplayType) 0);
	if (display == EGL_NO_DISPLAY) {
		fputs("Failed to get an EGLDisplay", stderr);
		return false;
	}

	EGLint major = 0;
	EGLint minor = 0;
	if (!eglInitialize(display, &major, &minor)) {
		puts("Failed to initialize the EGLDisplay");
		return false;
	} else {
        printf("EGL version %d.%d\n", major, minor);
    }
	
	int result = EGL_FALSE;

	result = eglBindAPI(EGL_OPENGL_ES_API);

	if (result != EGL_TRUE) {
		return false;
	}

	return true;
}

bool chooseEGLConfig(EGLDisplay display, EGLConfig& eglConfig) {

	const EGLint configurationAttributes[] = {
        EGL_RED_SIZE,           8,
        EGL_GREEN_SIZE,         8,
        EGL_BLUE_SIZE,          8,
        EGL_DEPTH_SIZE,         24,

        EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES2_BIT,
        EGL_NONE
	};

	EGLint configsReturned;
	if (!eglChooseConfig(display, configurationAttributes, &eglConfig, 1, &configsReturned) || (configsReturned != 1)) {
		fputs("Failed to choose a suitable config.", stderr);
		return false;
	}

	return true;
}

bool createEGLSurface(EGLDisplay display, EGLConfig eglConfig, EGLSurface& eglSurface) {

	eglSurface = eglCreateWindowSurface(display, eglConfig, (EGLNativeWindowType) 0, NULL);

	if (!testEGLError("eglCreateWindowSurface")) {
        return false;
    }

	return true;
}

bool setupEGLContext(EGLDisplay display, EGLConfig eglConfig, EGLSurface eglSurface, EGLContext& context) {

	eglBindAPI(EGL_OPENGL_ES_API);
	if (!testEGLError("eglBindAPI"))
        return false;

	EGLint contextAttributes[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	context = eglCreateContext(display, eglConfig, NULL, contextAttributes);
	if (!testEGLError("eglCreateContext")) {
        return false;
    }

	eglMakeCurrent(display, eglSurface, eglSurface, context);
	
	if (!testEGLError("eglMakeCurrent")) {
        return false;
    }

	return true;
}

bool testEGLError(const char* last_called) {

	EGLint last_error = eglGetError();
	if (last_error != EGL_SUCCESS) {
		printf("%s failed (%x).\n", last_called, last_error);
		return false;
	}

	return true;
}

bool testGLError(const char* last_called) {

	GLenum last_error = glGetError();
	if (last_error != GL_NO_ERROR) {
		printf("%s failed (%x).\n",last_called, last_error);
		return false;
	}

	return true;
}

