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

void gl_init() {
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthRangef(0.0f, 1.0f);
}

int main(int argc, char *argv[]) {

    EGLDisplay display = NULL;
    EGLConfig config = NULL;
    EGLSurface surface = NULL;
    EGLContext context = NULL;

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
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, , screen_width, screen_height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_color_buffer, 0);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture, 0);

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
          // position            // color            // texcoord   // normal
        { {-0.5f,  0.5f, -0.5f}, {1.0f, 0.5f, 0.0f}, {0.0f, 0.0f}, { 0.0f,  0.0f, -1.0f} },
        { {-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.5f}, {0.0f, 1.0f}, { 0.0f,  0.0f, -1.0f} },
        { { 0.5f,  0.5f,  0.5f}, {0.5f, 0.0f, 1.0f}, {1.0f, 1.0f}, { 0.0f,  0.0f, -1.0f} },
        { { 0.5f,  0.5f,  0.5f}, {0.5f, 0.0f, 1.0f}, {1.0f, 1.0f}, { 0.0f,  0.0f, -1.0f} },
        { { 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, { 0.0f,  0.0f, -1.0f} },
        { {-0.5f,  0.5f, -0.5f}, {1.0f, 0.5f, 0.0f}, {0.0f, 0.0f}, { 0.0f,  0.0f, -1.0f} },

        { {-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.5f}, {0.0f, 1.0f}, { 0.0f,  0.0f,  1.0f} },
        { {-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 0.5f}, {0.0f, 1.0f}, { 0.0f,  0.0f,  1.0f} },
        { { 0.5f, -0.5f,  0.5f}, {0.5f, 0.0f, 1.0f}, {1.0f, 1.0f}, { 0.0f,  0.0f,  1.0f} },
        { { 0.5f, -0.5f,  0.5f}, {0.5f, 0.0f, 1.0f}, {1.0f, 1.0f}, { 0.0f,  0.0f,  1.0f} },
        { { 0.5f,  0.5f,  0.5f}, {0.5f, 0.0f, 1.0f}, {1.0f, 1.0f}, { 0.0f,  0.0f,  1.0f} },
        { {-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.5f}, {0.0f, 1.0f}, { 0.0f,  0.0f,  1.0f} },

        { { 0.5f,  0.5f,  0.5f}, {0.5f, 0.0f, 1.0f}, {1.0f, 1.0f}, {-1.0f,  0.0f,  0.0f} },
        { { 0.5f, -0.5f,  0.5f}, {0.5f, 0.0f, 1.0f}, {1.0f, 1.0f}, {-1.0f,  0.0f,  0.0f} },
        { { 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, {-1.0f,  0.0f,  0.0f} },
        { { 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, {-1.0f,  0.0f,  0.0f} },
        { { 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, {-1.0f,  0.0f,  0.0f} },
        { { 0.5f,  0.5f,  0.5f}, {0.5f, 0.0f, 1.0f}, {1.0f, 1.0f}, {-1.0f,  0.0f,  0.0f} },

        { { 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, { 1.0f,  0.0f,  0.0f} },
        { { 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, { 1.0f,  0.0f,  0.0f} },
        { {-0.5f, -0.5f, -0.5f}, {1.0f, 0.5f, 0.0f}, {0.0f, 0.0f}, { 1.0f,  0.0f,  0.0f} },
        { {-0.5f, -0.5f, -0.5f}, {1.0f, 0.5f, 0.0f}, {0.0f, 0.0f}, { 1.0f,  0.0f,  0.0f} },
        { {-0.5f,  0.5f, -0.5f}, {1.0f, 0.5f, 0.0f}, {0.0f, 0.0f}, { 1.0f,  0.0f,  0.0f} },
        { { 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, { 1.0f,  0.0f,  0.0f} },

        { {-0.5f,  0.5f, -0.5f}, {1.0f, 0.5f, 0.0f}, {0.0f, 0.0f}, { 0.0f, -1.0f,  0.0f} },
        { {-0.5f, -0.5f, -0.5f}, {1.0f, 0.5f, 0.0f}, {0.0f, 0.0f}, { 0.0f, -1.0f,  0.0f} },
        { {-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 0.5f}, {0.0f, 1.0f}, { 0.0f, -1.0f,  0.0f} },
        { {-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 0.5f}, {0.0f, 1.0f}, { 0.0f, -1.0f,  0.0f} },
        { {-0.5f,  0.5f, -0.5f}, {1.0f, 0.5f, 0.0f}, {0.0f, 0.0f}, { 0.0f, -1.0f,  0.0f} },
        { {-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.5f}, {0.0f, 1.0f}, { 0.0f, -1.0f,  0.0f} },

        { {-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 0.5f}, {0.0f, 1.0f}, { 0.0f,  1.0f,  0.0f} },
        { {-0.5f, -0.5f, -0.5f}, {1.0f, 0.5f, 0.0f}, {0.0f, 0.0f}, { 0.0f,  1.0f,  0.0f} },
        { { 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, { 0.0f,  1.0f,  0.0f} },
        { { 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, { 0.0f,  1.0f,  0.0f} },
        { { 0.5f, -0.5f,  0.5f}, {0.5f, 0.0f, 1.0f}, {1.0f, 1.0f}, { 0.0f,  1.0f,  0.0f} },
        { {-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 0.5f}, {0.0f, 1.0f}, { 0.0f,  1.0f,  0.0f} },
    });

    Model light({
        { {-0.5f,  0.5f, -0.5f}, {1.0f, 0.5f, 0.0f}, {0.0f, 0.0f} },
        { {-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.5f}, {0.0f, 1.0f} },
        { { 0.5f,  0.5f,  0.5f}, {0.5f, 0.0f, 1.0f}, {1.0f, 1.0f} },
        { { 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} },
        { {-0.5f, -0.5f, -0.5f}, {1.0f, 0.5f, 0.0f}, {0.0f, 0.0f} },
        { {-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 0.5f}, {0.0f, 1.0f} },
        { { 0.5f, -0.5f,  0.5f}, {0.5f, 0.0f, 1.0f}, {1.0f, 1.0f} },
        { { 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} }
    },
    (std::vector<uint16_t>) { 
        0, 1, 2, 2, 3, 0,
        1, 5, 6, 6, 2, 1,
        2, 6, 7, 7, 3, 2,
        3, 7, 4, 4, 0, 3,
        0, 4, 5, 5, 1, 0,
        5, 4, 7, 7, 6, 5
    });

    Shader simple_shader("simple.vert", "simple.frag");
    simple_shader.bind_attrib(0, "position");
    simple_shader.bind_attrib(1, "color");
    simple_shader.bind_attrib(2, "tex_coord");

    Shader lighting_shader("lighting.vert", "lighting.frag");
    lighting_shader.bind_attrib(0, "position");
    lighting_shader.bind_attrib(3, "normal");

    Shader light_shader("light.vert", "light.frag");
    light_shader.bind_attrib(0, "position");

    Shader pp_shader("postprocessing.vert", "postprocessing.frag");
    pp_shader.bind_attrib(0, "position");
    pp_shader.bind_attrib(1, "color");
    pp_shader.bind_attrib(2, "tex_coord");

    if (argc == 2) {
        frames = strtol(argv[1], 0, 10);
    }

    printf("%d frames, coming up.\n", frames);

    auto start = std::chrono::high_resolution_clock::now();

    glm::vec3 light_pos = glm::vec3(1.0f);
    glm::vec3 light_col = glm::vec3(1.0f);

    for (int i = 0; i < frames; i++) {

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto current = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(current - start).count();

        glm::mat4 view = glm::lookAt(
                glm::vec3(0,1,5),
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

        light_shader.use();

        light.load_identity();
        light.translate(light_pos);
        light.scale(0.5f);
        mvp = projection * view * light.get_model_matrix();
        light_shader.set_vec3("light_color", light_col);
        light_shader.set_mat4("mvp", mvp);
        light.draw();

        lighting_shader.use();

        cube.load_identity();
        cube.translate(glm::vec3(0.0f));
        mvp = projection * view * cube.get_model_matrix();
        lighting_shader.set_vec3("light_position", light_pos);
        lighting_shader.set_vec3("light_color", light_col);
        lighting_shader.set_vec3("object_color", glm::vec3(1.0f, 0.5f, 0.31f));
        lighting_shader.set_mat4("model", cube.get_model_matrix());
        lighting_shader.set_mat4("mvp", mvp);
        cube.draw();

        /*
        checkerboard.load_identity();
        checkerboard.translate(glm::vec3(0));
        checkerboard.rotate(-90.0f, glm::vec3(1,0,0));
        checkerboard.scale(glm::vec3(50.0f));
        mvp = projection * view * checkerboard.get_model_matrix();
        simple_shader.set_mat4("mvp", mvp);
        simple_shader.set_float("tex_scale", 10.0);
        checkerboard.draw();

        cube.load_identity();
        cube.translate(glm::vec3(0, 0.5f, 0));
        cube.rotate(time * -45.0f, glm::vec3(0,1,0));
        mvp = projection * view * cube.get_model_matrix();
        simple_shader.set_mat4("mvp", mvp);
        simple_shader.set_float("tex_scale", 1.0);
        cube.draw();
        */

        glBindFramebuffer(GL_FRAMEBUFFER, 0);   // default framebuffer
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, texture_color_buffer);
        
        pp_shader.use();
        // pp_shader.set_float("time", time);
        
        screen.draw();

        if (!eglSwapBuffers(display, surface)) {
            testEGLError("eglSwapBuffers");
        }

    }

    puts("Done!");

    // TODO: clean up all buffers when done 

	return 0;
}
