#version 100

precision highp float;

attribute vec3 position;
attribute vec3 color;
attribute vec2 tex_coord;

varying vec3 outColor;
varying vec2 outTex;

uniform mat4 mvp;

void main(void) {
    outColor = color;
    outTex = tex_coord;

    gl_Position = mvp * vec4(position, 1.0);
}
