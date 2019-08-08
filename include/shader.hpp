#ifndef _SHADER_H
#define _SHADER_H

#include <GLES2/gl2.h>
#include <string>

class Shader {
private:
    GLuint handle;
    GLuint vertex_shader, fragment_shader;

public:
    Shader(std::string vs, std::string fs);
    GLuint load_shader(const std::string &, GLenum shader_type);
    void bind_attrib(GLuint index, const GLchar *name);
    void link();
    void use();
    void print_compile_status(GLenum shader_type);
    void print_link_status();
};

#endif
