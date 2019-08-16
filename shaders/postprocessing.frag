#version 300 es

precision highp float;

in vec2 outTex;
out vec4 fragColor;

uniform sampler2D screen_texture;
uniform float time;

// normal
vec4 unchanged(void) {
    return texture(screen_texture, outTex);
}

// negative color
vec4 negative(void) {
    return vec4(vec3(1.0 - texture(screen_texture, outTex)), 1.0);
}

// grayscale
vec4 grayscale(void) {
    vec4 frag = texture(screen_texture, outTex);
    float average = (frag.r + frag.g + frag.b) / 3.0;
    return vec4(vec3(average), 1.0);
}

// sharpen
vec4 sharpen(float _offset) {

    vec2 offsets[9] = vec2[](
         vec2(-_offset,  _offset),   // top-left
         vec2( 0.0f, _offset),   // top-center
         vec2( _offset,  _offset),   // top-right
         vec2(-_offset,  0.0f),  // center-left
         vec2( 0.0f, 0.0f),  // center-center
         vec2( _offset, 0.0f),   // center-right
         vec2(-_offset, -_offset),   // bottom-left
         vec2( 0.0f, -_offset),  // bottom-center
         vec2( _offset, -_offset)   // bottom-right 
    );

    float kernel[9] = float[](
        -1.0, -1.0, -1.0,
        -1.0,  9.0, -1.0,
        -1.0, -1.0, -1.0
    );
    
    vec3 sampled_tex[9];
    for (int i = 0; i < 9; i++) {
        sampled_tex[i] = vec3(texture(screen_texture, outTex.st + offsets[i]));
    }

    vec3 color = vec3(0.0);
    for (int i = 0; i < 9; i++) {
        color += sampled_tex[i] * kernel[i];
    }

    return vec4(color, 1.0);
}

vec4 blur(float _offset) {

    // sharpen
    vec2 offsets[9] = vec2[](
         vec2(-_offset,  _offset),   // top-left
         vec2( 0.0f, _offset),   // top-center
         vec2( _offset,  _offset),   // top-right
         vec2(-_offset,  0.0f),  // center-left
         vec2( 0.0f, 0.0f),  // center-center
         vec2( _offset, 0.0f),   // center-right
         vec2(-_offset, -_offset),   // bottom-left
         vec2( 0.0f, -_offset),  // bottom-center
         vec2( _offset, -_offset)   // bottom-right 
    );

    // blur
    float kernel[9] = float[](
        1.0/16.0, 2.0/16.0, 1.0/16.0,
        2.0/16.0, 4.0/16.0, 2.0/16.0,
        1.0/16.0, 2.0/16.0, 1.0/16.0
    );

    vec3 sampled_tex[9];
    for (int i = 0; i < 9; i++) {
        sampled_tex[i] = vec3(texture(screen_texture, outTex.st + offsets[i]));
    }

    vec3 color = vec3(0.0);
    for (int i = 0; i < 9; i++) {
        color += sampled_tex[i] * kernel[i];
    }

    return vec4(color, 1.0);
}

vec4 edge_detection(float _offset) {

    // sharpen
    vec2 offsets[9] = vec2[](
         vec2(-_offset,  _offset),   // top-left
         vec2( 0.0f, _offset),   // top-center
         vec2( _offset,  _offset),   // top-right
         vec2(-_offset,  0.0f),  // center-left
         vec2( 0.0f, 0.0f),  // center-center
         vec2( _offset, 0.0f),   // center-right
         vec2(-_offset, -_offset),   // bottom-left
         vec2( 0.0f, -_offset),  // bottom-center
         vec2( _offset, -_offset)   // bottom-right 
    );

    // edge detection
    float kernel[9] = float[](
         1.0,  1.0,  1.0,
         1.0, -8.0,  1.0,
         1.0,  1.0,  1.0
    );

    vec3 sampled_tex[9];
    for (int i = 0; i < 9; i++) {
        sampled_tex[i] = vec3(texture(screen_texture, outTex.st + offsets[i]));
    }

    vec3 color = vec3(0.0);
    for (int i = 0; i < 9; i++) {
        color += sampled_tex[i] * kernel[i];
    }

    return vec4(color, 1.0);
}

vec4 toon(float _offset) {

    // sharpen
    vec2 offsets[9] = vec2[](
         vec2(-_offset,  _offset),   // top-left
         vec2( 0.0f, _offset),   // top-center
         vec2( _offset,  _offset),   // top-right
         vec2(-_offset,  0.0f),  // center-left
         vec2( 0.0f, 0.0f),  // center-center
         vec2( _offset, 0.0f),   // center-right
         vec2(-_offset, -_offset),   // bottom-left
         vec2( 0.0f, -_offset),  // bottom-center
         vec2( _offset, -_offset)   // bottom-right 
    );

    // edge detection
    float kernel[9] = float[](
        -1.0, -2.0, -1.0,
         0.0,  0.0,  0.0,
         1.0,  2.0,  1.0
    );

    vec3 sampled_tex[9];
    for (int i = 0; i < 9; i++) {
        sampled_tex[i] = vec3(texture(screen_texture, outTex.st + offsets[i]));
    }

    vec3 color = vec3(0.0);
    for (int i = 0; i < 9; i++) {
        color += sampled_tex[i] * kernel[i];
    }

    return vec4(color, 1.0);
}

void main (void) {
    const float os = 1.0 / 300.0;
    float strength = abs(sin(time)) * os;

    fragColor = blur(10.0 * strength);
}
