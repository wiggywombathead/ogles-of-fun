#version 300

precision highp float;

varying vec2 outTex;

uniform sampler2D screen_texture;

void main (void) {
    // normal: 
    // gl_FragColor = texture2D(screen_texture, outTex);

    // inverted:
    // gl_FragColor = vec4(vec3(1.0 - texture2D(screen_texture, outTex)), 1.0);

    // grayscale:
    vec4 frag = texture2D(screen_texture, outTex);
    float average = (frag.r + frag.g + frag.b) / 3.0;
    gl_FragColor = vec4(average, average, average, 1.0);

    // const float off = 1.0 / 300.0;

    // vec2 offsets[9] = vec2[](
    //      vec2(-off,  off),   // top-left
    //      vec2( 0.0f, off),   // top-center
    //      vec2( off,  off),   // top-right
    //      vec2(-off,  0.0f),  // center-left
    //      vec2( 0.0f, 0.0f),  // center-center
    //      vec2( off, 0.0f),   // center-right
    //      vec2(-off, -off),   // bottom-left
    //      vec2( 0.0f, -off),  // bottom-center
    //      vec2( off, -off)   // bottom-right 
    // );

    // float kernel[9] = float[](
    // -1, -1, -1,
    // -1, 9, -1,
    // -1, -1, -1
    // );

    // vec3 sampled_tex[9];
    // for (int i = 0; i < 9; i++) {
    //     sampled_tex[i] = vec3(texture2D(screen_texture, outTex.st + offsets[i]));
    // }

    vec3 color = vec3(0.0);
    // for (int i = 0; i < 9; i++) {
    //     color += sampled_tex[i] * kernel[i];
    // }

    gl_FragColor = vec4(color, 1.0);
}
