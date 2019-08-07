attribute vec3 position;
attribute vec3 color;
attribute vec2 tex_coord;

varying vec3 outTex;
varying vec3 outColor;

void main(void) {
    outColor = color;
    outTex = tex_coord;
    gl_Position = vec4(position, 1.0);
}
