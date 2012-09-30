uniform sampler2D texture;
varying vec2 coord;

void main(void) {
  gl_FragColor = texture2D(texture, vec2((coord.x + 1.0)/2.0, (coord.y + 1.0)/2.0));
}