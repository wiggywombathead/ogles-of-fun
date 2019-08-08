#ifndef _MODEL_H
#define _MODEL_H

#include "vertex.hpp"

#include <GLES2/gl2.h>
#include <string>
#include <vector>

class Model {
private:
    std::vector<Vertex> mesh;
    std::vector<uint16_t> indices;

    GLuint vertex_buffer;
    GLuint index_buffer;
    GLuint texture;

    bool indexed = false;

public:
    Model(std::vector<Vertex>);
    Model(std::vector<Vertex>, std::string);
    Model(std::vector<Vertex>, std::vector<uint16_t>);
    Model(std::vector<Vertex>, std::vector<uint16_t>, std::string);
    GLuint init_vertex_buffer();
    GLuint init_index_buffer();
    GLuint load_texture(const std::string filename);
    void draw();
};

#endif
