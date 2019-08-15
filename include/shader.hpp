#ifndef _SHADER_H
#define _SHADER_H

#include <GLES2/gl2.h>
#include <glm/glm.hpp>
#include <string>


class Shader {
private:
    GLuint program;
    GLuint vertex_shader, fragment_shader;

public:
    /* initialisation */
    Shader(std::string vs, std::string fs);
    GLuint load_shader(const std::string, GLenum shader_type);
    void bind_attrib(GLuint index, const GLchar *name);
    void link();
    void use();

    /* set values in shader */
    void set_mat4(std::string, glm::mat4);
    void set_float(std::string, float);
    void set_bool(std::string, bool);

    /* util */
    void print_compile_status(GLuint shader, std::string filename);
    void print_link_status();
};

#endif
