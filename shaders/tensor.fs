#version 400

uniform sampler2D Image;
uniform sampler2D hatch1;
uniform sampler2D hatch2;
uniform sampler2D g;
uniform int imgW;
uniform int imgH;
uniform int size;
uniform float denom;
uniform mat3 sobelx;
uniform mat3 sobely;
uniform bool edgeAligned;

in vec2 uv;
out vec4 FragColor;

float gray(vec4 col){
  return col.r*.11 + col.g*.59 + col.b*.11;
}
void main()
{
  vec4 c;
  vec2 coord;
  vec4 sumx = vec4(0,0,0,1);
  vec4 sumy = vec4(0,0,0,1);
  //apply sobel 3x3 kernel for partial derivatives
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

  // eigen values
  float E = gray(sumx*sumx);
  float F = gray(sumx*sumy);
  float G = gray(sumy*sumy);
  vec4 ST = vec4(E,F,F,G);

  // float ST[4] = {sumx*sumx, sumx*sumy, sumx*sumy, sumy*sumy};
  float e1 = (ST.x + ST.w + sqrt(pow(ST.x-ST.w,2)+4*pow(ST.y,2)))/2.0;
  float e2 = (ST.x + ST.w - sqrt(pow(ST.x-ST.w,2)+4*pow(ST.y,2)))/2.0;
  float a = (e1-e2)/(e1+e2);

  // eigen vectors 
  // first eigen vector (should be dirrection of least change)
  vec2 t1 = vec2(e1-ST.x,-1*ST.y);
  // float t1[2] = {e1-ST.x,-1*ST.y};
  // float t1[2] = {ST.y,e1-ST.x};
  float mag1 = sqrt(pow(t1.x,2) + pow(t1.y,2));

  // second eigen vector (should be dirrection of most change)
  vec2 t2 = vec2(-1*ST.y,e2-ST.w); 
  // float t2[2] = {-1*ST.y,e2-ST.w}; 
  // float t2[2] = {e2-ST.w,ST.y}; 
  float mag2 = sqrt(pow(t2.x,2) + pow(t2.y,2));
  
  if(edgeAligned){
    // remap
    // t1.x = (((t1.x/mag1) + 1)/2.0);
    // t1.y = (((t1.y/mag1) + 1)/2.0);
    FragColor = vec4((((t1.x/mag1) + 1)/2.0),(((t1.y/mag1) + 1)/2.0), mag1,1);
    // FragColor = vec4((((t1.x/mag1) + 1)/2.0),(((t1.y/mag1) + 1)/2.0), mag1,1);
  }else{
    // remap
    // t2.x = (((t2.x/mag2) + 1)/2.0);
    // t2.y = (((t2.y/mag2) + 1)/2.0);
    FragColor = vec4((((t2.x/mag2) + 1)/2.0),(((t2.y/mag2) + 1)/2.0), a,1);
  }
}


