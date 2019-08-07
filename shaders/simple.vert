attribute vec3 position;
attribute vec3 color;

varying vec3 outColor;

void main(void) {
    outColor = color;
    gl_Position = vec4(position, 1.0);
}
