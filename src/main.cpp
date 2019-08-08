// #define DYNAMICGLES_NO_NAMESPACE
// #define DYNAMICEGL_NO_NAMESPACE
// #include <DynamicGles.h>

#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

#include <glm/gtc/matrix_transform.hpp>

#include "egl_init.hpp"
#include "model.hpp"

int screen_width, screen_height;

std::vector<Model> models;

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

void print_shader_status(GLuint shader, GLenum shader_type) {

	GLint is_compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);

	if (!is_compiled) {

		int len, written;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

        char log[len];
		glGetShaderInfoLog(shader, len, &written, log);

        std::string type_string;
        switch (shader_type) {
        case GL_VERTEX_SHADER:
            type_string = "vertex";
            break;
        case GL_FRAGMENT_SHADER:
            type_string = "fragment";
            break;
        }

		printf("%s shader failed to compile: %s\n", type_string.c_str(), log);
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

bool render(GLuint shader_program, EGLDisplay eglDisplay, EGLSurface eglSurface) {

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static auto start = std::chrono::high_resolution_clock::now();
    auto current = std::chrono::high_resolution_clock::now();

    float time = std::chrono::duration<float, std::chrono::seconds::period>(current - start).count();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(
            model,
            time * glm::radians(0.0f),
            glm::vec3(0,1,0)
        );

    glm::mat4 view = glm::lookAt(
            glm::vec3(0,0,3),
            glm::vec3(0,0,0),
            glm::vec3(0,1,0)
        );

    glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float) screen_width / (float) screen_height,
            0.1f,
            100.0f
        );

    glm::mat4 mvp = projection * view * model;

    int mvp_handle = glGetUniformLocation(shader_program, "mvp");
    glUniformMatrix4fv(mvp_handle, 1, GL_FALSE, &mvp[0][0]);

    for (auto model : models) {
        model.draw();
    }

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

void gl_init() {
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthRangef(0.0f, 1.0f);
}

int main(int /*argc*/, char** /*argv*/) {

    EGLDisplay display = NULL;
    EGLConfig config = NULL;
    EGLSurface surface = NULL;
    EGLContext context = NULL;

	// Handles for the two shaders used to draw the triangle, and the program handle which combines them.
	GLuint vertex_shader = 0, fragment_shader = 0;
	GLuint shader_program = 0;

	// A vertex buffer object to store our model data.
	GLuint vertexBuffer = 0;

    if (!egl_init(display, config, surface, context)) {
        egl_cleanup(display);
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &screen_width);
    eglQuerySurface(display, surface, EGL_HEIGHT, &screen_height);

    gl_init();

    Model checkerboard({
        { {-0.5f,  0.5f,  0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} },    // top left
        { {-0.5f, -0.5f,  0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} },    // bottom left
        { { 0.5f, -0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f} },    // bottom right

        { { 0.5f, -0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f} },    // bottom right
        { { 0.5f,  0.5f,  0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} },    // top right
        { {-0.5f,  0.5f,  0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} }     // top left
    }, "../tex/checkerboard.jpg");

    Model dickbutt({
        { {-1.0f,  1.0f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },    // top left
        { {-1.0f,  0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f} },    // bottom left
        { { 0.0f,  0.0f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f} },    // bottom right

        { { 0.0f,  0.0f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f} },    // bottom right
        { { 0.0f,  1.0f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} },    // top right
        { {-1.0f,  1.0f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },    // top left
    }, "../tex/dickbutt.jpg");
    
    Model checkerboard2(
            {
                { { 0.0f,  0.0f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} },    // top left
                { { 0.0f, -1.0f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} },    // bottom left
                { { 1.0f, -1.0f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f} },    // bottom right
                { { 1.0f,  0.0f,  0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} },    // top right
            }, 
            { 0, 1, 2, 2, 3, 0 },
            "../tex/planks.jpg"
        );

    models.push_back(checkerboard);
    models.push_back(dickbutt);
    models.push_back(checkerboard2);

	if (!initializeShaders(vertex_shader, fragment_shader, shader_program))
        egl_cleanup(display);

    for (;;) {

		if (!render(shader_program, display, surface))
            break;

    }

	destroy_state(vertex_shader, fragment_shader, shader_program, vertexBuffer);

	return 0;
}
