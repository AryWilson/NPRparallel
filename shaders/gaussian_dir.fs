#version 400

uniform sampler2D Image;
uniform sampler2D hatch1;
uniform sampler2D hatch2;
uniform sampler2D tensor;
uniform sampler2D g;
uniform int imgW;
uniform int imgH;
uniform int size;
// uniform float denom;

in vec2 uv;
out vec4 FragColor;


void main()
{
  
  vec4 cx;

  //find edge of gaussian kernel
  int index = 0;
  int half_ = int(floor(size/2));
  // init sum
  vec3 sum = vec3(0,0,0);
  // init tensor dir
  vec3 vec = texture(tensor,vec2(uv.x,uv.y)).xyz;
  // get the x,y components of the first eigen vector
  float vec_x = vec.r*2.0 -1;
  float vec_y = vec.g*2.0 -1;
  float col_x = uv.x;
  float col_y = uv.y;
  int cutoff = 10;
  for (int i = -half_; i<=half_; i++){
    // move base don tensor
      col_x = col_x + (float(i))*vec_x; // move right some
      col_y = col_y + (float(i))*vec_y;
      // get x dir color and scale by gaussian
      cx = texture(Image,vec2(col_x,col_y));

      // get gaussian value and scale by gaussuan
      vec4 gb = texture(Image,vec2(float(index)/size + .5*(1/size),.5));
      sum += (cx*(gb.b)).rgb;

      //next gaussian kernel value
      index++;
      // updte tensor and vectors
      vec = texture(tensor,vec2(col_x,col_y)).xyz;
      // get the x,y components of the first eigen vector
      vec_x = (vec.r)*(2.0) - 1.0;
      vec_y = (vec.g)*(2.0) - 1.0;
  }
  sum = sum/(index/2.0);
  FragColor = vec4(sum.xyz,1);
}
