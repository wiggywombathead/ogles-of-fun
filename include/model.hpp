#ifndef _MODEL_H
#define _MODEL_H

#include "vertex.hpp"

#include <GLES2/gl2.h>
#include <string>
#include <vector>

class Model {
private:
    std::vector<Vertex> mesh;

    GLuint vertex_buffer;
    GLuint index_buffer;
    GLuint texture;

public:
    Model(std::vector<Vertex>);
    Model(std::vector<Vertex>, std::string);
    GLuint init_vertex_buffer();
    GLuint load_texture(const std::string filename);
    void draw();
};

#endif
