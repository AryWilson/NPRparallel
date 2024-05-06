#version 400

uniform sampler2D Image;
uniform sampler2D hatch1;
uniform sampler2D hatch2;
uniform int imgW;
uniform int imgH;
uniform mat3 sobelx;
uniform mat3 sobely;

in vec2 uv;
out vec4 FragColor;


void main()
{
  //get image color
  vec4 c;
  vec2 coord;
  vec4 sumx = vec4(0,0,0,1);
  vec4 sumy = vec4(0,0,0,1);
  for (int i = 0; i<3; i++){
    for (int j = 0; j<3; j++){
      coord = vec2((uv.x*imgW + i-1)/imgW,(uv.y*imgH + j-1)/imgH);
      c = texture(Image,coord);
      sumx += c*sobelx[i][j];
      sumy += c*sobely[i][j];
    }

  }
  sumx = sumx/4.0f;
  sumy = sumy/4.0f;

  FragColor = vec4(sumx.xyz+sumy.xyz,1);
}
