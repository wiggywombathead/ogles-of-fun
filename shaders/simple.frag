precision mediump float;

varying vec3 outColor;

void main (void) {
    gl_FragColor = vec4(1.0 - outColor, 1.0);
}
