#include "shader.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>

Shader::Shader(std::string vs, std::string fs) {

    std::string base = "../shaders/";

    vertex_shader = load_shader(base + vs, GL_VERTEX_SHADER);
    fragment_shader = load_shader(base + fs, GL_FRAGMENT_SHADER);

    handle = glCreateProgram();

	glAttachShader(handle, vertex_shader);
	glAttachShader(handle, fragment_shader);
}

std::vector<char> read_shader(const std::string &filename) {

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

GLuint Shader::load_shader(const std::string &filename, GLenum shader_type) {

    GLuint shader = glCreateShader(shader_type);

    std::vector<char> source = read_shader(filename);
    std::string src(source.begin(), source.end());
    const char *char_array = src.c_str();

    GLint length = source.size();
    glShaderSource(shader, 1, &char_array, &length);
    glCompileShader(shader);

    print_compile_status(shader_type);

    return shader;
}

void Shader::bind_attrib(GLuint index, const GLchar *name) {
    glBindAttribLocation(handle, index, name);
}

void Shader::link() {
    glLinkProgram(handle);

    print_link_status();
}

void Shader::use() {
    glUseProgram(handle);
}

void Shader::set_mat4(std::string variable, glm::mat4 matrix) {
    GLint matrix_handle = glGetUniformLocation(handle, variable.c_str());
    glUniformMatrix4fv(matrix_handle, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::print_compile_status(GLenum shader_type) {

    GLuint shader;
    if (shader_type == GL_VERTEX_SHADER) 
        shader = vertex_shader;
    else if (shader_type == GL_FRAGMENT_SHADER)
        shader = fragment_shader;

	GLint is_compiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);

    printf("is_compiled == %d\n", is_compiled);

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

		printf("WANK %s shader failed to compile: %s\n", type_string.c_str(), log);
	}

}

void Shader::print_link_status() {

	GLint isLinked;
	glGetProgramiv(handle, GL_LINK_STATUS, &isLinked);

	if (!isLinked) {

		int len, written;
		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &len);

        char log[len];
		glGetProgramInfoLog(handle, len, &written, log);

		printf("%s", len > 1 ? log : "Failed to link shader program.");
	}

}

