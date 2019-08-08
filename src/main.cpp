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
#include "shader.hpp"

int screen_width, screen_height;

std::vector<Model> models;

/*
bool initializeShaders(GLuint& vertex_shader, GLuint &fragment_shader, GLuint& shader_program) {

    std::string vshader_path = base + "shaders/simple.vert";
    std::string fshader_path = base + "shaders/simple.frag";

    vertex_shader = load_shader(vshader_path, GL_VERTEX_SHADER);
    fragment_shader = load_shader(fshader_path, GL_FRAGMENT_SHADER);

	shader_program = glCreateProgram();

	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);

    print_program_status(shader_program);

	glUseProgram(shader_program);

	if (!testGLError("glUseProgram")) {
        return false;
    }

	return true;
}
*/

bool render(GLuint shader_program, EGLDisplay eglDisplay, EGLSurface eglSurface) {

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static auto start = std::chrono::high_resolution_clock::now();
    auto current = std::chrono::high_resolution_clock::now();

    float time = std::chrono::duration<float, std::chrono::seconds::period>(current - start).count();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(
            model,
            glm::vec3(0.0f) + sinf(time) * glm::vec3(0,0,2)
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

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

GLuint create_framebuffer() {
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_width, screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    // glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, , screen_width, screen_height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // now attach the texture to the framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
        
    }
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

    Model bricks({
            { { 0.0f,  0.0f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} },    // top left
            { { 0.0f, -1.0f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} },    // bottom left
            { { 1.0f, -1.0f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f} },    // bottom right
            { { 1.0f,  0.0f,  0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} },    // top right
        }, 
        { 0, 1, 2, 2, 3, 0 }, 
        "../tex/bricks.png"
    );

    models.push_back(checkerboard);
    models.push_back(dickbutt);
    models.push_back(bricks);

    Shader simple("simple.vert", "simple.frag");
    simple.bind_attrib(0, "position");
    simple.bind_attrib(1, "color");
    simple.bind_attrib(2, "tex_coord");
    simple.link();
    simple.use();

    for (;;) {

		if (!render(shader_program, display, surface))
            break;

    }

	destroy_state(vertex_shader, fragment_shader, shader_program, vertexBuffer);

    // TODO: clean up all buffers when done 

	return 0;
}
