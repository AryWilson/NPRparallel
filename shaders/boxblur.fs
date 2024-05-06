#version 400

uniform sampler2D Image;
uniform sampler2D hatch1;
uniform sampler2D hatch2;
uniform int imgW;
uniform int imgH;

in vec2 uv;
out vec4 FragColor;


void main()
{
  //get image color
  vec4 c;
  vec2 coord;
  vec4 sum = vec4(0,0,0,1);
  for (int i = 0; i<3; i++){
    for (int j = 0; j<3; j++){
      coord = vec2((uv.x*imgW + i-1)/imgW,(uv.y*imgH + j-1)/imgH);
      c = texture(Image,coord);
      sum += c;
    }

  }
  sum = sum/9.0f;

  FragColor = vec4(sum.xyz,1);
}
