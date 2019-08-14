#version 100

precision highp float;

varying vec3 outColor;
varying vec2 outTex;

uniform sampler2D tex_sampler;

void main (void) {
    // fragOut = textureLod(tex_sampler, 25.0 * outTex, 7.0);
    gl_FragColor = texture2D(tex_sampler, 100.0 * outTex);
}

