#version 300 es

precision highp float;

in vec2 outTex;
out vec4 fragColor;

uniform sampler2D screen_texture;

void main (void) {
    // normal: 
    // fragColor = texture2D(screen_texture, outTex);

    // inverted:
    // fragColor = vec4(vec3(1.0 - texture(screen_texture, outTex)), 1.0);

    // grayscale:
    // vec4 frag = texture(screen_texture, outTex);
    // float average = (frag.r + frag.g + frag.b) / 3.0;
    // fragColor = vec4(average, average, average, 1.0);

    const float off = 1.0 / 300.0;

    // sharpen
    vec2 offsets[9] = vec2[](
         vec2(-off,  off),   // top-left
         vec2( 0.0f, off),   // top-center
         vec2( off,  off),   // top-right
         vec2(-off,  0.0f),  // center-left
         vec2( 0.0f, 0.0f),  // center-center
         vec2( off, 0.0f),   // center-right
         vec2(-off, -off),   // bottom-left
         vec2( 0.0f, -off),  // bottom-center
         vec2( off, -off)   // bottom-right 
    );

    // sharpen
    float sharpen_kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );
    
    // blur
    float blur_kernel[9] = float[](
        1.0/16.0, 2.0/16.0, 1.0/16.0,
        2.0/16.0, 4.0/16.0, 2.0/16.0,
        1.0/16.0, 2.0/16.0, 1.0/16.0
    );

    // edge detection
    float ed_kernel[9] = float[](
         1,  1,  1,
         1, -8,  1,
         1,  1,  1
    );

    vec3 sampled_tex[9];
    for (int i = 0; i < 9; i++) {
        sampled_tex[i] = vec3(texture(screen_texture, outTex.st + offsets[i]));
    }

    vec3 color = vec3(0.0);
    for (int i = 0; i < 9; i++) {
        color += sampled_tex[i] * ed_kernel[i];
    }

    fragColor = vec4(color, 1.0);
}
