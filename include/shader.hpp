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
    Shader();
    Shader(std::string vs, std::string fs);
    GLuint load_shader(const std::string, GLenum shader_type);

    bool is_compiled(GLuint);
    bool is_linked(void);

    void bind_attrib(GLuint index, const GLchar *name);
    void use();

    /* set values in shader */
    void set_bool(std::string, bool);
    void set_float(std::string, float);
    void set_vec3(std::string, glm::vec3);
    void set_mat4(std::string, glm::mat4);

    /* util */
    void print_compile_error(GLuint shader, std::string filename);
    void print_link_error();
};

#endif
