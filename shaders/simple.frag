#version 100

precision mediump float;

varying vec3 outColor;
varying vec2 outTex;

uniform sampler2D tex_sampler;

void main (void) {
    // gl_FragColor = vec4(outColor, 1.0);
    gl_FragColor = texture2D(tex_sampler, 100.0 * outTex);// * vec4(outColor, 1.0);
}

