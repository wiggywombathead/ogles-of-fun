#version 300 es

precision highp float;

in vec3 outNorm;
in vec3 fragPos;

out vec4 fragColor;

uniform vec3 light_position;
uniform vec3 light_color;
uniform vec3 object_color;
uniform vec3 eye_position;

void main(void) {

    // ambient
    float ambient_strength = 0.4;

    // diffuse
    vec3 norm = normalize(outNorm);
    vec3 light_dir = normalize(light_position - fragPos);
    float diff = max(dot(norm, light_dir), 0.0);

    // specular
    float specular_strength = 0.5;
    vec3 view_dir = normalize(eye_position - fragPos);

    // phong
    // vec3 reflect_dir = reflect(-light_dir, norm);
    // float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 4.0);

    // blinn-phong
    vec3 halfway_dir = normalize(light_dir + view_dir);
    float spec = pow(max(dot(norm, halfway_dir), 0.0), 32.0);
    
    vec3 ambient = ambient_strength * light_color;
    vec3 diffuse = diff * light_color;
    vec3 specular = specular_strength * spec * light_color;

    vec3 result = (ambient + diffuse + specular) * object_color;

    fragColor = vec4(result, 1.0);
}
