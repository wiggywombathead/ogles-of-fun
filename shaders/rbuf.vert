#version 300

precision highp float;

attribute vec3 position;
attribute vec2 tex_coord;

varying vec2 outTex;

void main(void) {
    outTex = tex_coord;
    gl_Position = vec4(position.xy, 0.0, 1.0);
}
