#include "model.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <cstdint>
#include <iostream>

Model::Model(std::vector<Vertex> vertices) {
    mesh = vertices;
    init_vertex_buffer();
}

Model::Model(std::vector<Vertex> vertices, std::vector<uint16_t> indices) : Model(vertices) {
    this->indices = indices;
    indexed = true;
    init_index_buffer();
}

Model::Model(std::vector<Vertex> vertices, std::string texpath) : Model(vertices) {
    texture = load_texture(texpath);
}

Model::Model(std::vector<Vertex> vertices, std::vector<uint16_t> indices, std::string texpath) : Model(vertices, indices) {
    texture = load_texture(texpath);
}

GLuint Model::init_vertex_buffer() {
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh.size(), mesh.data(), GL_STATIC_DRAW);

    return vertex_buffer;
}

GLuint Model::init_index_buffer() {
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * indices.size(), indices.data(), GL_STATIC_DRAW);

    return index_buffer;
}

GLuint Model::load_texture(const std::string filename) {

    int width, height, channels;

    stbi_uc *pixels = stbi_load(
            filename.c_str(),
            &width,
            &height,
            &channels,
            STBI_rgb_alpha
        );

    if (pixels == nullptr) {
        fprintf(stderr, "Could not find texture %s\n", filename.c_str());
        return 0;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // TODO
    // glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(pixels);

    return texture;
}

void Model::draw() {

    glBindTexture(GL_TEXTURE_2D, texture);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    // position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

    // colour
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) (offsetof(Vertex, color)));

    // texture coordinate
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) (offsetof(Vertex, texcoord)));

    if (indexed) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, mesh.size());
    }
}
