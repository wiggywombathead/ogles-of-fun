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

    program = glCreateProgram();

	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
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

GLuint Shader::load_shader(const std::string filename, GLenum shader_type) {

    GLuint shader = glCreateShader(shader_type);

    std::vector<char> source = read_shader(filename);
    std::string src(source.begin(), source.end());
    const char *char_array = src.c_str();

    GLint length = source.size();
    glShaderSource(shader, 1, &char_array, &length);
    glCompileShader(shader);

    print_compile_status(shader, filename);

    return shader;
}

void Shader::bind_attrib(GLuint index, const GLchar *name) {
    glBindAttribLocation(program, index, name);
}

void Shader::link() {
    glLinkProgram(program);

    print_link_status();
}

void Shader::use() {
    glUseProgram(program);
}

void Shader::set_mat4(std::string variable, glm::mat4 matrix) {
    GLint handle = glGetUniformLocation(program, variable.c_str());
    glUniformMatrix4fv(handle, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::set_float(std::string variable, float value) {
    GLint handle = glGetUniformLocation(program, variable.c_str());
    glUniform1f(handle, value);
}

void Shader::set_bool(std::string variable, bool value) {
    GLint handle = glGetUniformLocation(program, variable.c_str());
    glUniform1i(handle, value);
}

void Shader::print_compile_status(GLuint shader, std::string filename) {

	GLint is_compiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);

	if (is_compiled == GL_FALSE) {

		int len, written;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

        char log[len];
		glGetShaderInfoLog(shader, len, &written, log);

		printf("%s: %s\n", filename.c_str(), log);
	}

}

void Shader::print_link_status() {

	GLint isLinked;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);

	if (!isLinked) {

		int len, written;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

        char log[len];
		glGetProgramInfoLog(program, len, &written, log);

		printf("%s", len > 1 ? log : "Failed to link shader program.");
	}

}

