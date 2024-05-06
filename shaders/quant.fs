#version 400

uniform sampler2D Image;
uniform sampler2D hatch1;
uniform sampler2D hatch2;
uniform int colors;

in vec2 uv;
out vec4 FragColor;


void main()
{
  //get image color
  vec4 c = texture(Image, uv);

  //quant into 6 colors
  vec3 q = vec3((round(c.x*6)/6.0f),(round(c.y*6)/6.0f),(round(c.z*6)/6.0f)) ;
  //   float sum = c.x*.17 + c.y*.66 + c.z*.17;

  FragColor = vec4(q,1);
}
