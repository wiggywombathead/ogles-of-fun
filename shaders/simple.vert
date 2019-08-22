#version 300 es

precision lowp float;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 tex_coord;

out vec3 outColor;
out vec2 outTex;

uniform mat4 mvp;

void main(void) {
    outColor = color;
    outTex = tex_coord;

    gl_Position = mvp * vec4(position, 1.0);
}
