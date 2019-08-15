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

    Model passion({
        { {-0.5f,  0.5f,  0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },    // top left
        { {-0.5f, -0.5f,  0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f} },    // bottom left
        { { 0.5f, -0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f} },    // bottom right
        { { 0.5f,  0.5f,  0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} },    // top right
        }, 
        { 0, 1, 2, 2, 3, 0 }, 
        "../tex/passion.jpg"
    );

    models.push_back(bricks);
    models.push_back(cube);
    // models.push_back(checkerboard);
    // models.push_back(dickbutt);

    Shader simple_shader("simple.vert", "simple.frag");
    simple_shader.bind_attrib(0, "position");
    simple_shader.bind_attrib(1, "color");
    simple_shader.bind_attrib(2, "tex_coord");
    simple_shader.link();

    Shader pp_shader("postprocessing.vert", "postprocessing.frag");
    pp_shader.bind_attrib(0, "position");
    pp_shader.bind_attrib(1, "color");
    pp_shader.bind_attrib(2, "tex_coord");
    pp_shader.link();

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

        passion.load_identity();
        passion.translate(glm::vec3(0));
        passion.rotate(time * -90.0f, glm::vec3(0,0,1));
        passion.scale(glm::vec3(5.0f));
        mvp = projection * view * passion.get_model_matrix();
        simple_shader.set_mat4("mvp", mvp);
        simple_shader.set_float("tex_scale", 1.0);
        passion.draw();

        // cube.load_identity();
        // cube.translate(glm::vec3(0));
        // cube.rotate(-90.0f, glm::vec3(1,0,0));
        // cube.scale(2.0f);
        // mvp = projection * view * cube.get_model_matrix();
        // simple_shader.set_mat4("mvp", mvp);
        // simple_shader.set_float("tex_scale", 1.0);
        // cube.draw();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);   // default framebuffer
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        pp_shader.use();
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, texture_color_buffer);
        
        pp_shader.set_float("time", time);
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
