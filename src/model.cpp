#include "model.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Model::Model(std::vector<Vertex> vertices) {
    mesh = vertices;
}

Model::Model(std::vector<Vertex> vertices, std::string texpath) {
    mesh = vertices;
    texture = load_texture(texpath);

    init_vertex_buffer();

}

GLuint Model::init_vertex_buffer() {

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh.size(), mesh.data(), GL_STATIC_DRAW);

    return vertex_buffer;
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

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    stbi_image_free(pixels);

    return texture;
}

void Model::draw() {

    glBindTexture(GL_TEXTURE_2D, texture);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    // position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

    // // colour
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) (offsetof(Vertex, color)));

    // // texture coordinate
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) (offsetof(Vertex, texcoord)));

    glDrawArrays(GL_TRIANGLES, 0, mesh.size());
}
