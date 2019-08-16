#version 300 es

precision highp float;

in vec3 outNorm;
in vec3 fragPos;

out vec4 fragColor;

uniform vec3 light_position;
uniform vec3 light_color;
uniform vec3 object_color;

void main(void) {

    // ambient
    float ambient_strength = 0.4;

    // diffuse
    vec3 norm = normalize(outNorm);
    vec3 light_dir = normalize(light_position - fragPos);
    float diff = max(dot(norm, light_dir), 0.0);
    
    vec3 ambient = ambient_strength * light_color;
    vec3 diffuse = diff * light_color;

    vec3 result = (ambient + diffuse) * object_color;

    fragColor = vec4(result, 1.0);
}
