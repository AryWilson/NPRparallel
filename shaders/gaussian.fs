#version 400

uniform sampler2D Image;
uniform sampler2D hatch1;
uniform sampler2D hatch2;
uniform sampler2D g;
uniform int imgW;
uniform int imgH;
uniform int size;
// uniform float denom;

in vec2 uv;
out vec4 FragColor;


void main()
{
  
  vec4 cx,cy;
  vec2 coord;
  //init dirrectional sums as 0
  vec3 sumx = vec3(0,0,0);
  vec3 sumy = vec3(0,0,0);

  //find edge of gaussian kernel... efficient, unecessary
  int index= 0;
  int half_ = int(floor(size/2));
  for (int i = -half_; i<=half_; i++){
    for (int j = -half_; j<=half_; j++){
      // get x dir color and scale by gaussian
      float coordx = (uv.x*imgW + i)/imgW;
      cx = texture(Image,vec2(coordx,uv.y));

      // get gaussian value
      vec4 gb = texture(Image,vec2(float(index)/size + .5*(1/size),.5));

      sumx += (cx*(gb.b)).rgb;

      // get y dir col and scale by gaussuan...
      float coordy = (uv.y*imgH + j)/imgH;
      cy = texture(Image,vec2(uv.x,coordy));
      sumy += (cy*(gb.b)).rgb;

      index++;
    }

  }
  sumx = sumx/index;
  sumy = sumy/index;
  FragColor = vec4(sumx.xyz+sumy.xyz,1);
}
