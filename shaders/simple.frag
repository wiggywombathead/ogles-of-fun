precision mediump float;

varying vec3 outColor;
varying vec2 outTex;

uniform sampler2D _texture;

void main (void) {
    // gl_FragColor = vec4(outColor, 1.0);
    gl_FragColor = texture2D(_texture, outTex);
}

