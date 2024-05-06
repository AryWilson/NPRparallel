#version 400

uniform sampler2D Image;
uniform sampler2D hatch1;
uniform sampler2D hatch2;
uniform bool color;

in vec2 uv;
out vec4 FragColor;

//todo better rand fn
float rand(float min, float max){
  return min + 1 + (max-min)*sin(min*max);
}

void main()
{
  // float ran = 5;
  //get image color
  vec4 c = texture(Image, uv);
  // get hatch1 color
  float ran = rand(7.1,8.9);
  vec4 h1 = texture(hatch1, uv*ran);
  // get hatch2 color
  ran = rand(7.2,8.7);
  vec4 h2 = texture(hatch2, uv*ran);
  // get hatch3 color
  ran = rand(7.2,8.6);
  vec4 h3 = texture(hatch1, vec2(uv.y*ran,uv.x*ran));
  // get hatch4 color
  ran = rand(7.4,8.5);
  vec4 h4 = texture(hatch2, vec2(uv.y*ran,uv.x*ran));

  // gray
  // vec3 q = vec3((round(c.x*6)/6.0f),(round(c.y*6)/6.0f),(round(c.z*6)/6.0f)) ;
  float sum = c.x*.17 + c.y*.66 + c.z*.17;
  vec3 q = vec3(1,1,1);

  //GRAY
  if (sum > (5/6.0)){ // white
    q = vec3(1,1,1);
  } else if (sum > (4/6.0)){ // hatch 1
    q = h1.xyz;
  } else if (sum > (3/6.0)){ // hatch 1 + hacth 2
    // float r = (h1.x > h2.x ? h2.x : h1.x);
    float r = min(h1.x,h2.x);
    q = vec3(r,r,r);
  } else if (sum > (2/6.0)){ // hatch 1 + hacth 2 + hacth 3
    // float r = (h1.x>h2.x? (h1.x>h3.x? h1.x: h3.x): (h2.x>h3.x? h2.x: h3.x));
    float r = min(h1.x,min(h2.x,h3.x));
    q = vec3(r,r,r);
  } else if (sum > (1/6.0)){ // hatch 1 + hacth 2 + hacth 3 + hacth 4
    float r = min(min(h1.x,h2.x),min(h3.x,h4.x));
    // float r = (h1.x>h2.x? (h1.x>h3.x? h1.x: h3.x): (h2.x>h3.x? h2.x: h3.x));
    // r = (r>h4.x? r: h4.x);
    q = vec3(r,r,r);
  } else { // black
    q = vec3(0,0,0);
  }

  // COLOR
  if (color){
    q = q+c.xyz;
  }
  // FragColor = vec4(sum,sum,sum,1);
  // FragColor = h1;
  FragColor = vec4(q,1);
}
