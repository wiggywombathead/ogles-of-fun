#version 300 es

precision highp float;

out vec4 fragColor;

uniform vec3 light_color;

void main(void) {
    fragColor = vec4(light_color, 1.0);
}
