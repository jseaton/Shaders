uniform sampler2D texture;
//uniform float width;
//uniform float height;
varying vec2 coord;

void main(void) {
     float sum = 0;

     for (float i=-1.0;i<=1.0;i+=1.0) {
     	 for (float j=-1.0;j<=1.0;j+=1.0) {
	     if (texture2D(texture, coord + vec2(i,j)/2048.0).r > 0.5) {
	     	sum += 1.0;
	     }
         }
     }
     
     float col = 0.0;
     
     if (sum == 3.0 || (sum == 4.0 && texture2D(texture, coord).r > 0.5)) {
     	col = 1.0;
     }
     gl_FragColor = vec4(col, 0.0, 0.0, 1.0);
}