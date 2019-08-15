#version 300 es

precision highp float;

in vec3 outColor;
in vec2 outTex;

out vec4 fragColor;

uniform sampler2D tex_sampler;

void main (void) {
    // fragOut = textureLod(tex_sampler, 25.0 * outTex, 7.0);
    fragColor = texture(tex_sampler, 100.0 * outTex);
}
