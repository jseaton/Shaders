uniform float scale;
uniform vec2 offset;
varying vec2 coord;

void main(void) {
  vec2 z = vec2(0.0, 0.0);
  vec2 c = vec2(offset.x+coord.x*scale,offset.y+coord.y*scale);
  
  float col = 0.0;
  float i;
  for (i=0.0;i<400000.0;i++) {
    z = vec2(z.x*z.x-z.y*z.y, 2*z.x*z.y) + c;
    if (z.x*z.x + z.y*z.y > 4.0) {
      col = max(0.5, 1.0-i/400.0);
      break;
    }
  }
  gl_FragColor = vec4(col,col,col,1.0);
}
