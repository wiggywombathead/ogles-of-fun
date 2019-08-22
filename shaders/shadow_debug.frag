#version 300 es

precision highp float;

in vec2 outTex;

out vec4 fragColor;

uniform sampler2D depth_map;
uniform float near_plane;
uniform float far_plane;

float linearise_depth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main() {
    float depth = texture(depth_map, outTex).r;
    fragColor = vec4(vec3(depth), 1.0);
}
