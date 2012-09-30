attribute vec2 position;
varying vec2 coord;

void main(void) {
  coord = position;
  gl_Position = vec4(position, 1.0, 1.0);
}