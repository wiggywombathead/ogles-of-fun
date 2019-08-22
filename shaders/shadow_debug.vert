#version 300 es

precision highp float;

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 tex_coords;

out vec2 outTex;

void main() {
    outTex = tex_coords;
    gl_Position = vec4(position, 1.0);
}
