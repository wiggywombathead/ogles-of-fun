#version 100

precision highp float;

varying vec3 outColor;
varying vec2 outTex;

uniform sampler2D tex_sampler;
uniform float tex_scale;

void main (void) {
    gl_FragColor = texture2D(tex_sampler, tex_scale * outTex);
}
