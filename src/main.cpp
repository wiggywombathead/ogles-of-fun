// #define DYNAMICGLES_NO_NAMESPACE
// #define DYNAMICEGL_NO_NAMESPACE
// #include <DynamicGles.h>

#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include <vector>
#include <stdio.h>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

// Index to bind the attributes to vertex shaders
const unsigned int VertexArray	= 0;

bool testEGLError(const char* functionLastCalled) {

	EGLint lastError = eglGetError();
	if (lastError != EGL_SUCCESS) {
		printf("%s failed (%x).\n", functionLastCalled, lastError);
		return false;
	}

	return true;
}

bool testGLError(const char* functionLastCalled) {

	GLenum lastError = glGetError();
	if (lastError != GL_NO_ERROR) {
		printf("%s failed (%x).\n", functionLastCalled, lastError);
		return false;
	}

	return true;
}

bool createEGLDisplay(EGLDisplay& eglDisplay) {

	eglDisplay = eglGetDisplay((EGLNativeDisplayType)0);
	if (eglDisplay == EGL_NO_DISPLAY) {
		printf("Failed to get an EGLDisplay");
		return false;
	}

	EGLint eglMajorVersion = 0;
	EGLint eglMinorVersion = 0;
	if (!eglInitialize(eglDisplay, &eglMajorVersion, &eglMinorVersion)) {
		printf("Failed to initialize the EGLDisplay");
		return false;
	}
	
	int result = EGL_FALSE;

	result = eglBindAPI(EGL_OPENGL_ES_API);

	if (result != EGL_TRUE) {
		return false;
	}

	return true;
}

bool chooseEGLConfig(EGLDisplay eglDisplay, EGLConfig& eglConfig) {

	const EGLint configurationAttributes[] = {
        EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES2_BIT,
        EGL_NONE
	};

	EGLint configsReturned;
	if (!eglChooseConfig(eglDisplay, configurationAttributes, &eglConfig, 1, &configsReturned) || (configsReturned != 1)) {
		printf("Failed to choose a suitable config.");
		return false;
	}

	return true;
}

bool createEGLSurface(EGLDisplay eglDisplay, EGLConfig eglConfig, EGLSurface& eglSurface) {
	eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, (EGLNativeWindowType)0, NULL);

	if (!testEGLError("eglCreateWindowSurface"))
        return false;

	return true;
}

bool setupEGLContext(EGLDisplay eglDisplay, EGLConfig eglConfig, EGLSurface eglSurface, EGLContext& context) {

	eglBindAPI(EGL_OPENGL_ES_API);
	if (!testEGLError("eglBindAPI"))
        return false;

	EGLint contextAttributes[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	context = eglCreateContext(eglDisplay, eglConfig, NULL, contextAttributes);
	if (!testEGLError("eglCreateContext"))
        return false;

	eglMakeCurrent(eglDisplay, eglSurface, eglSurface, context);
	
	if (!testEGLError("eglMakeCurrent"))
        return false;

	return true;
}

bool initializeBuffer(GLuint& vertexBuffer) {

	GLfloat vertexData[] = { 
        -0.4f, -0.4f,  0.0f, 1.0f, 0.0f, 0.0f,
         0.4f, -0.4f,  0.0f, 0.0f, 1.0f, 0.0f,
         0.0f,  0.4f,  0.0f, 0.0f, 0.0f, 1.0f,
    };

	glGenBuffers(1, &vertexBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	if (!testGLError("glBufferData"))
        return false;

	return true;
}

void print_shader_status(GLuint shader, GLenum shader_type) {

	GLint is_compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);

	if (!is_compiled) {

		int len, written;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

		std::vector<char> log;
        log.resize(len);
		glGetShaderInfoLog(shader, len, &written, log.data());

        std::string type_string;
        switch (shader_type) {
        case GL_VERTEX_SHADER:
            type_string = "vertex";
            break;
        case GL_FRAGMENT_SHADER:
            type_string = "fragment";
            break;
        }

		printf("%s shader: %s", type_string.c_str(), len > 1 ? log.data() : "failed to compile.");
	}

}

void print_program_status(GLuint program) {

	GLint isLinked;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);

	if (!isLinked) {

		int infoLogLength, charactersWritten;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		std::vector<char> infoLog; infoLog.resize(infoLogLength);
		glGetProgramInfoLog(program, infoLogLength, &charactersWritten, infoLog.data());

		printf("%s", infoLogLength > 1 ? infoLog.data() : "Failed to link shader program.");
	}

}

std::vector<char> read_file(const std::string &filename) {

    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("could not open file");
    }

    size_t filesize = (size_t) file.tellg();
    std::vector<char> buffer(filesize);

    file.seekg(0);
    file.read(buffer.data(), filesize);

    std::cout << "Read GLSL shader " << filename << " of size " << filesize << " bytes" << std::endl;

    file.close();
    return buffer;

}

GLuint load_shader(const std::string &filename, GLenum shader_type) {

    GLuint shader = glCreateShader(shader_type);

    std::vector<char> source = read_file(filename);
    std::string src(source.begin(), source.end());
    const char *char_array = src.c_str();

    GLint length = source.size();
    glShaderSource(shader, 1, &char_array, &length);
    glCompileShader(shader);

    print_shader_status(shader, shader_type);

    return shader;
}

bool initializeShaders(GLuint& vertex_shader, GLuint &fragment_shader, GLuint& shader_program) {

    // std::string base = "examples/OpenGLES/01_HelloAPI/";
    std::string base = "../";

    std::string vshader_path = base + "shaders/simple.vert";
    std::string fshader_path = base + "shaders/simple.frag";
    
    vertex_shader = load_shader(vshader_path, GL_VERTEX_SHADER);
    fragment_shader = load_shader(fshader_path, GL_FRAGMENT_SHADER);

	shader_program = glCreateProgram();

	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);

	glBindAttribLocation(shader_program, 0, "position");
	glBindAttribLocation(shader_program, 1, "color");
	glLinkProgram(shader_program);

    print_program_status(shader_program);

	glUseProgram(shader_program);

	if (!testGLError("glUseProgram"))
        return false;

	return true;
}

bool renderScene(GLuint shaderProgram, EGLDisplay eglDisplay, EGLSurface eglSurface) {

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT);

	int matrixLocation = glGetUniformLocation(shaderProgram, "transformationMatrix");

	const float transformationMatrix[] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, transformationMatrix);
	if (!testGLError("glUniformMatrix4fv"))
        return false;

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (GLvoid *) (3*sizeof(float)));

	if (!testGLError("glVertexAttribPointer"))
        return false;

	glDrawArrays(GL_TRIANGLES, 0, 3);
	if (!testGLError("glDrawArrays"))
        return false;

    /*
    if(isGlExtensionSupported("GL_EXT_discard_framebuffer")) {
        GLenum invalidateAttachments[2];
        invalidateAttachments[0] = GL_DEPTH_EXT;
        invalidateAttachments[1] = GL_STENCIL_EXT;

        glDiscardFramebufferEXT(GL_FRAMEBUFFER, 2, &invalidateAttachments[0]);
        if (!testGLError("glDiscardFramebufferEXT"))
            return false;
    }
    */

    if (!eglSwapBuffers(eglDisplay, eglSurface)) {
        testEGLError("eglSwapBuffers");
        return false;
    }

    return true;
}

void deInitializeGLState(GLuint vertex_shader, GLuint fragment_shader, GLuint shader_program, GLuint vertexBuffer) {

	glDeleteShader(fragment_shader);
	glDeleteShader(vertex_shader);
	glDeleteProgram(shader_program);

	glDeleteBuffers(1, &vertexBuffer);
}

void releaseEGLState(EGLDisplay eglDisplay) {

	eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

	eglTerminate(eglDisplay);
}

int main(int /*argc*/, char** /*argv*/) {

    EGLDisplay eglDisplay = NULL;
    EGLConfig eglConfig = NULL;
    EGLSurface eglSurface = NULL;
    EGLContext context = NULL;

	// Handles for the two shaders used to draw the triangle, and the program handle which combines them.
	GLuint fragment_shader = 0, vertex_shader = 0;
	GLuint shader_program = 0;

	// A vertex buffer object to store our model data.
	GLuint vertexBuffer = 0;

	if (!createEGLDisplay(eglDisplay))
        goto cleanup;

	if (!chooseEGLConfig(eglDisplay, eglConfig))
        goto cleanup;

	if (!createEGLSurface(eglDisplay, eglConfig, eglSurface))
        goto cleanup;

	if (!setupEGLContext(eglDisplay, eglConfig, eglSurface, context))
        goto cleanup;

	if (!initializeBuffer(vertexBuffer))
        goto cleanup;

	if (!initializeShaders(vertex_shader, fragment_shader, shader_program))
        goto cleanup;

	for (int i = 0; i < 800; ++i) {

		if (!renderScene(shader_program, eglDisplay, eglSurface))
            break;

    }

	deInitializeGLState(vertex_shader, fragment_shader, shader_program, vertexBuffer);

cleanup:
	releaseEGLState(eglDisplay);
	return 0;
}
