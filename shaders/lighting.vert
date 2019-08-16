#version 300 es

precision highp float;

layout (location = 0) in vec3 position;
layout (location = 3) in vec3 normal;

out vec3 outNorm;
out vec3 fragPos;

uniform mat4 mvp;
uniform mat4 model;

void main(void) {
    outNorm = normal;
    fragPos = vec3(model * vec4(position, 1.0));
    gl_Position = mvp * vec4(position, 1.0);
}
