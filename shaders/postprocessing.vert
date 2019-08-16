#version 300 es

precision highp float;

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 tex_coord;

out vec2 outTex;

uniform mat4 mvp;

void main(void) {
    outTex = tex_coord;
    gl_Position = vec4(position.xy, 0.0, 1.0);
}
