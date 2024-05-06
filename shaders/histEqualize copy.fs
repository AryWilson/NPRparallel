#version 400

uniform sampler2D Image;
uniform sampler2D hatch1;
uniform sampler2D hatch2;
uniform sampler2D cdf;
uniform int imgW;
uniform int imgH;
uniform float[21] g;
uniform int size;
uniform float denom;
uniform int min;

in vec2 uv;
out vec4 FragColor;

void main()
{

  vec4 col = texture(Image,vec2(uv.x,uv.y));

  // get the array
  float gr = col.r*.11 + col.g*.59 + col.b*.11;
  
  vec4 c = texture(cdf,vec2(gr,0));
  int r = int(round(c.x*255));
  int g = int(round(c.y*255));
  int b = int(round(c.z*255));
  int a = int(round(c.w*255));
  // wont work because int is sized so it's too small... boolean switch between engative and postive
  int large = (r<<24)|(g<<16)|(b<<8)|(a);

  float gray = round((large-min)*(255+1)/(imgW*imgH-min));

  float gy = (gray)/255.0;
  FragColor = vec4(vec3(gy),1);

}
