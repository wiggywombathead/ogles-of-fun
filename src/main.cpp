// #define DYNAMICGLES_NO_NAMESPACE
// #define DYNAMICEGL_NO_NAMESPACE
// #include <DynamicGles.h>

#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include <vector>
#include <cstdio>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "egl_init.hpp"

GLuint load_texture(const std::string filename) {

    int width, height, channels;

    stbi_uc *pixels = stbi_load(
            filename.c_str(),
            &width,
            &height,
            &channels,
            STBI_rgb_alpha
        );

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    stbi_image_free(pixels);

    return texture;
}

bool initializeBuffer(GLuint& vertexBuffer) {

	GLfloat vertexData[] = { 
        // XYZ                // COLOR
        -0.5f, -0.5f,  0.0f,  1.0f, 0.0f, 0.0f,//   0.0f, 0.0f,    // bottom left
         0.5f, -0.5f,  0.0f,  0.0f, 1.0f, 1.0f,//   0.0f, 0.0f,    // bottom right
         0.5f,  0.5f,  0.0f,  0.0f, 0.0f, 1.0f,//   0.0f, 0.0f,    // top right

         0.5f,  0.5f,  0.0f,  0.0f, 0.0f, 1.0f,//   0.0f, 0.0f,    // top right
        -0.5f,  0.5f,  0.0f,  1.0f, 1.0f, 0.0f,//   0.0f, 0.0f,    // top left
        -0.5f, -0.5f,  0.0f,  1.0f, 0.0f, 0.0f,//   0.0f, 0.0f,    // bottom left
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
	glBindAttribLocation(shader_program, 2, "tex_coord");
	glLinkProgram(shader_program);

    print_program_status(shader_program);

	glUseProgram(shader_program);

	if (!testGLError("glUseProgram"))
        return false;

	return true;
}

bool render(GLuint shaderProgram, EGLDisplay eglDisplay, EGLSurface eglSurface) {

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT);

    /*
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
    */

    // position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), 0);

    // colour
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (GLvoid *) (3*sizeof(float)));

    // texture coordinate
    // glEnableVertexAttribArray(2);
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (GLvoid *) (6*sizeof(float)));

	if (!testGLError("glVertexAttribPointer"))
        return false;

	//glDrawArrays(GL_TRIANGLES, 0, 6);
	if (!testGLError("glDrawArrays"))
        return false;

    if (!eglSwapBuffers(eglDisplay, eglSurface)) {
        testEGLError("eglSwapBuffers");
        return false;
    }

    return true;
}

void destroy_state(GLuint vertex_shader, GLuint fragment_shader, GLuint shader_program, GLuint vertexBuffer) {

	glDeleteShader(fragment_shader);
	glDeleteShader(vertex_shader);
	glDeleteProgram(shader_program);

	glDeleteBuffers(1, &vertexBuffer);
}

int main(int /*argc*/, char** /*argv*/) {

    EGLDisplay display = NULL;
    EGLConfig config = NULL;
    EGLSurface surface = NULL;
    EGLContext context = NULL;

	// Handles for the two shaders used to draw the triangle, and the program handle which combines them.
	GLuint fragment_shader = 0, vertex_shader = 0;
	GLuint shader_program = 0;

	// A vertex buffer object to store our model data.
	GLuint vertexBuffer = 0;

    if (!egl_init(display, config, surface, context))
        egl_cleanup(display);

	if (!initializeBuffer(vertexBuffer))
        egl_cleanup(display);

	if (!initializeShaders(vertex_shader, fragment_shader, shader_program))
        egl_cleanup(display);

	for (int i = 0; i < 800; ++i) {

		if (!render(shader_program, display, surface))
            break;

    }

	destroy_state(vertex_shader, fragment_shader, shader_program, vertexBuffer);

	return 0;
}
