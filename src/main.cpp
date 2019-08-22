#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

#include <unistd.h>

#include <glm/gtc/matrix_transform.hpp>

#include "egl_init.hpp"
#include "model.hpp"
#include "shader.hpp"

#define DEFAULT_FRAMES 1000

enum {
    ESSL_VERSION_100,
    ESSL_VERSION_300
};

int screen_width, screen_height;
int frames = DEFAULT_FRAMES;
int shader_version = ESSL_VERSION_300;

std::vector<Model> models;

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

}

GLuint create_renderbuffer() {
    GLuint renderbuffer;
    glGenRenderbuffers(1, &renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screen_width, screen_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);
}

int main(int argc, char *argv[]) {

    EGLDisplay display = NULL;
    EGLConfig config = NULL;
    EGLSurface surface = NULL;
    EGLContext context = NULL;

    if (!egl_init(display, config, surface, context)) {
        egl_cleanup(display);
    }


    printf("GLES version: %s\nShading Language version: %s\n",
            glGetString(GL_VERSION),
            glGetString(GL_SHADING_LANGUAGE_VERSION)
        );

    eglQuerySurface(display, surface, EGL_WIDTH, &screen_width);
    eglQuerySurface(display, surface, EGL_HEIGHT, &screen_height);

    gl_init();

    GLuint renderbuffer = create_renderbuffer();

    // create framebuffer
    GLuint postprocess_framebuffer;
    glGenFramebuffers(1, &postprocess_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, postprocess_framebuffer);    

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

    /*
    GLuint depth_map_framebuffer;
    glGenFramebuffers(1, &depth_map_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_framebuffer);

    GLuint depth_map;
    glGenTextures(1, &depth_map);
    glBindTexture(GL_TEXTURE_2D, depth_map);

    const int SHADOW_WIDTH = 1024;
    const int SHADOW_HEIGHT = 1024;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
    // glDrawBuffer(GL_NONE);  // tell OGL we are not going to render any color data
    // glReadBuffer(GL_NONE);  // tell OGL we are not going to render any color data
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    */

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

    int opt;
    while ((opt = getopt(argc, argv, "f:v:")) != -1) {
        switch (opt) {
        case 'f':
            frames = strtol(optarg, 0, 10);
            break;
        case 'v':
            shader_version = strtol(optarg, 0, 10);
            printf("Using ESSL version %d\n", shader_version);

            // select ESSL version
            switch (shader_version) {
            case 100:
                shader_version = ESSL_VERSION_100;
                break;
            case 300:
                shader_version = ESSL_VERSION_300;
                break;
            default:
                fprintf(
                        stderr,
                        "Could not recognise ESSL version %d. Defaulting to version 300.\n",
                        shader_version
                    );
                shader_version = ESSL_VERSION_300;
            }

            break;
        }
    }

    printf("%d frames, coming up.\n", frames);

    Shader simple_shader;
    if (shader_version == ESSL_VERSION_100) {
        Shader simple_100("simple_100.vert", "simple_100.frag");
        simple_shader = simple_100;
    } else if (shader_version == ESSL_VERSION_300) {
        Shader simple_300("simple.vert", "simple.frag");
        simple_shader = simple_300;
    }

    // Shader shadow_shader("shadowmap.vert", "shadowmap.frag");
    // Shader shadow_debug("shadow_debug.vert", "shadow_debug.frag");

    // Shader pp_shader("postprocessing.vert", "postprocessing.frag");

    // Shader lighting_shader("lighting.vert", "lighting.frag");
    // Shader light_shader("light.vert", "light.frag");

    auto start = std::chrono::high_resolution_clock::now();

    glm::vec3 light_pos = glm::vec3(1.0f);
    glm::vec3 light_col = glm::vec3(1.0f);
    glm::vec3 eye_position = glm::vec3(0, 1, 5);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    float near_plane = 0.1f, far_plane = 10.0f;

    for (int i = 0; i < frames; i++) {

        /*
        glm::mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        glm::mat4 light_view = glm::lookAt(
                glm::vec3(-2.0f, 4.0f, -1.0f), 
                glm::vec3( 0.0f, 0.0f,  0.0f), 
                glm::vec3( 0.0f, 1.0f,  0.0f)
            ); 

        glm::mat4 light_space_matrix = light_projection * light_view;

        shadow_shader.use();
        shadow_shader.set_mat4("light_space_matrix", light_space_matrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depth_map_framebuffer);
            glClear(GL_DEPTH_BUFFER_BIT);

            checkerboard.load_identity();
            shadow_shader.set_mat4("model", checkerboard.get_model_matrix());
            checkerboard.draw();

        glViewport(0, 0, screen_width, screen_height);
        glBindFramebuffer(GL_FRAMEBUFFER, postprocess_framebuffer);
            // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glClear(GL_COLOR_BUFFER_BIT);

            glDisable(GL_DEPTH_TEST);
            glBindTexture(GL_TEXTURE_2D, depth_map); // texture_color_buffer);

            shadow_debug.use();
            shadow_debug.set_float("near_plane", near_plane);
            shadow_debug.set_float("far_plane", far_plane);

            glBindTexture(GL_TEXTURE_2D, depth_map);

            screen.draw();
        */

        // glBindFramebuffer(GL_FRAMEBUFFER, postprocess_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto current = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(current - start).count();

        glm::mat4 view = glm::lookAt(
                eye_position,
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

        light_pos = glm::vec3(
                sinf(time),
                0.0f,
                1.5f
            );

        /*
        light_shader.use();

        light.load_identity();
        light.translate(light_pos);
        light.scale(0.25f);
        mvp = projection * view * light.get_model_matrix();
        light_shader.set_vec3("light_color", light_col);
        light_shader.set_mat4("mvp", mvp);
        light.draw();

        lighting_shader.use();

        cube.load_identity();
        cube.translate(glm::vec3(0.0f, 0.0f, -1.0f));
        mvp = projection * view * cube.get_model_matrix();
        lighting_shader.set_vec3("light_position", light_pos);
        lighting_shader.set_vec3("light_color", light_col);
        lighting_shader.set_vec3("object_color", glm::vec3(1.0f, 0.5f, 0.31f));
        lighting_shader.set_vec3("eye_position", eye_position);

        lighting_shader.set_mat4("model", cube.get_model_matrix());
        lighting_shader.set_mat4("mvp", mvp);
        cube.draw();
        */

        simple_shader.use();

        checkerboard.load_identity();
        checkerboard.translate(glm::vec3(0));
        checkerboard.rotate(-90.0f, glm::vec3(1,0,0));
        // checkerboard.rotate(time * 15.0f, glm::vec3(0,0,1));
        checkerboard.scale(50.0f);
        mvp = projection * view * checkerboard.get_model_matrix();
        simple_shader.set_mat4("mvp", mvp);
        simple_shader.set_float("tex_scale", 50.0);
        checkerboard.draw();

        /*
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, texture_color_buffer);

        pp_shader.use();
        pp_shader.set_float("time", time);
        screen.draw();
        */

        if (!eglSwapBuffers(display, surface)) {
            testEGLError("eglSwapBuffers");
        }

    }

    puts("Done!");

    // TODO: clean up all buffers when done 

	return 0;

}
