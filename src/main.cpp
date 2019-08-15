#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

#include <glm/gtc/matrix_transform.hpp>

#include "egl_init.hpp"
#include "model.hpp"
#include "shader.hpp"

#define DEFAULT_FRAMES 200

int screen_width, screen_height;
int frames = DEFAULT_FRAMES;

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

void render(Shader shader, EGLDisplay display, EGLSurface surface) {

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static auto start = std::chrono::high_resolution_clock::now();
    auto current = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(current - start).count();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    shader.use();

    glm::mat4 view = glm::lookAt(
            glm::vec3(0,2,10),
            glm::vec3(0,0,0),
            glm::vec3(0,1,0)
        );

    glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float) screen_width / (float) screen_height,
            0.1f,
            1000.0f
        );

    glm::mat4 mvp;

    if (time < 2.0f) {
        eglSwapBuffers(display, surface);
        return;
    }

    models[0].load_identity();

    float factor = time > 10.0f ? 10.0f : time;
    models[0].translate(glm::vec3(0,0,0));
    models[0].rotate(factor * -90.0f / 10.0f, glm::vec3(1,0,0));
    models[0].scale(glm::vec3(500.0f));

    mvp = projection * view * models[0].get_model_matrix();
    shader.set_mat4("mvp", mvp);
    models[0].draw();

    /*
    for (auto model : models) {
        model.draw();
    }
    */

    if (!eglSwapBuffers(display, surface)) {
        testEGLError("eglSwapBuffers");
    }

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

    // TODO: why not compile? glEnable(GL_FRAMEBUFFER_SRGB);
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

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void cleanup() {
    // glDeleteFramebuffers(1,
}

int main(int argc, char *argv[]) {

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

    // create framebuffer
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);    

    GLuint texture_color_buffer;
    glGenTextures(1, &texture_color_buffer);
    glBindTexture(GL_TEXTURE_2D, texture_color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_width, screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_color_buffer, 0);

    GLuint render_buffer;
    glGenRenderbuffers(1, &render_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, render_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screen_width, screen_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, render_buffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fputs("Framebuffer not complete", stderr);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    Model screen({
        { {-1.0f,  1.0f,  0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f} },
        { {-1.0f, -1.0f,  0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },
        { { 1.0f, -1.0f,  0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f} },
        { { 1.0f,  1.0f,  0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f} }
    }, (std::vector<uint16_t>) { 0, 1, 2, 2, 3, 0 });

    Model checkerboard({
        { {-0.5f,  0.5f,  0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} },    // top left
        { {-0.5f, -0.5f,  0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} },    // bottom left
        { { 0.5f, -0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f} },    // bottom right

        { { 0.5f, -0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f} },    // bottom right
        { { 0.5f,  0.5f,  0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} },    // top right
        { {-0.5f,  0.5f,  0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} }     // top left
    }, "../tex/checkerboard.jpg");

    Model cube({
        { {-0.5f,  0.5f, -0.5f}, {1.0f, 0.5f, 0.0f}, {0.0f, 0.0f} },
        { {-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.5f}, {0.0f, 1.0f} },
        { { 0.5f,  0.5f,  0.5f}, {0.5f, 0.0f, 1.0f}, {1.0f, 1.0f} },
        { { 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} },
        { {-0.5f, -0.5f, -0.5f}, {1.0f, 0.5f, 0.0f}, {0.0f, 0.0f} },
        { {-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 0.5f}, {0.0f, 1.0f} },
        { { 0.5f, -0.5f,  0.5f}, {0.5f, 0.0f, 1.0f}, {1.0f, 1.0f} },
        { { 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} }
    },
    { 
        0, 1, 2, 2, 3, 0,
        1, 5, 6, 6, 2, 1,
        2, 6, 7, 7, 3, 2,
        3, 7, 4, 4, 0, 3,
        0, 4, 5, 5, 1, 0,
        5, 4, 7, 7, 6, 5
    }, "../tex/planks.jpg");

    Model dickbutt({
        { {-0.5f,  0.5f,  0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },    // top left
        { {-0.5f, -0.5f,  0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f} },    // bottom left
        { { 0.5f, -0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f} },    // bottom right
        { { 0.5f,  0.5f,  0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} },    // top right
        },
        { 0, 1, 2, 2, 3, 0 },
        "../tex/dickbutt.jpg"
    );

    Model bricks({
        { {-0.5f,  0.5f,  0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },    // top left
        { {-0.5f, -0.5f,  0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f} },    // bottom left
        { { 0.5f, -0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f} },    // bottom right
        { { 0.5f,  0.5f,  0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} },    // top right
        }, 
        { 0, 1, 2, 2, 3, 0 }, 
        "../tex/bricks.png"
    );

    models.push_back(bricks);
    // models.push_back(checkerboard);
    // models.push_back(dickbutt);

    Shader simple_shader("simple.vert", "simple.frag");
    simple_shader.bind_attrib(0, "position");
    simple_shader.bind_attrib(1, "color");
    simple_shader.bind_attrib(2, "tex_coord");
    simple_shader.link();

    Shader screen_shader("rbuf.vert", "rbuf.frag");
    screen_shader.bind_attrib(0, "position");
    screen_shader.bind_attrib(1, "color");
    screen_shader.bind_attrib(2, "tex_coord");
    screen_shader.link();

    if (argc == 2) {
        frames = strtol(argv[1], 0, 10);
    }

    printf("%d frames, coming up.\n", frames);

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < frames; i++) {

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        simple_shader.use();

        auto current = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(current - start).count();

        glm::mat4 view = glm::lookAt(
                glm::vec3(0,0,10),
                glm::vec3(0,0,0),
                glm::vec3(0,1,0)
                );

        glm::mat4 projection = glm::perspective(
                glm::radians(45.0f),
                (float) screen_width / (float) screen_height,
                0.1f,
                1000.0f
                );

        glm::mat4 mvp;

        if (time < 2.0f) {
            eglSwapBuffers(display, surface);
            continue;
        }

        models[0].load_identity();

        float factor = time > 10.0f ? 10.0f : time;
        models[0].translate(glm::vec3(0,0,0));
        // models[0].rotate(-90.0f, glm::vec3(1,0,0));
        models[0].scale(glm::vec3(500.0f));

        mvp = projection * view * models[0].get_model_matrix();
        simple_shader.set_mat4("mvp", mvp);
        models[0].draw();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);   // default framebuffer
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        screen_shader.use();
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, texture_color_buffer);
        screen.draw();

        if (!eglSwapBuffers(display, surface)) {
            testEGLError("eglSwapBuffers");
        }

    }

	destroy_state(vertex_shader, fragment_shader, shader_program, vertexBuffer);

    puts("Done!");

    // TODO: clean up all buffers when done 

	return 0;
}
