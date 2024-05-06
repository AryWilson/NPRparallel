#version 400

uniform sampler2D Image;
uniform sampler2D hatch1;
uniform sampler2D hatch2;
uniform sampler2D cdf;
uniform int imgW;
uniform int imgH;

uniform int bins;
uniform int min;

in vec2 uv;
out vec4 FragColor;

void main()
{

  vec4 col = texture(Image,vec2(uv.x,uv.y));

  float gr = col.r*.11 + col.g*.59 + col.b*.11;

  // get the array
  vec4 c = texture(cdf,vec2(gr + .5*(1/256) ,.5));
  float large = (c.r)*(imgW*imgH);
  // FragColor = vec4(vec3(large),1);

  float gray = round(((large-min)*(255+1))/(imgW*imgH-min));

  float gy = (gray)/255.0;
  FragColor = vec4(vec3(gy),1);

}
