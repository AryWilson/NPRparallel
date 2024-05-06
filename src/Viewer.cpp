//--------------------------------------------------
// Author: Ary Wilson
// Date: 2/18/23
// Description: launches viewer
//--------------------------------------------------
#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include <vector>
#include "agl/window.h"
#include "image.h"
// #include "plymesh.h"
#include "osutils.h"
#include <cstring>
#include <bits/stdc++.h>
#include <time.h>
#include <loadvideo.h>
const int MAX_G = 30;
const int MAX_WINDOW = 600;


using namespace std;
using namespace glm;
using namespace agl;

struct Particle {
  glm::vec3 pos;// no change in x,y, change in z in shader
  glm::vec4 color;//changes
  float size;// changes
  float rot;//changes

};

struct kernel {
   int size;
   float denom;
   std::vector<float> kernel;
};

class Viewer : public Window
{
public:
   Viewer() : Window() {
      
   }

   void setup() {
      srand(time(0));
      frames = 0;
      time0 = 0.0;
      
      // get image to render
      int ind = 1; // keep track of loaded location
      textures = {"still3","bat","bench"};
      img.load("../textures/" + textures[0] + ".png", true);
      renderer.loadTexture(textures[0], img, ind++);

      //VIDEO LOAD 
      /*
      std::vector<std::string> videos = {"cow","ifh","squirrel"};
      const char* outfilename = "vid";

      if (LoadVideo(videos[0], video_frames)) {
        std::string outname = outfilename;
        outname += "-0.ppm";
        video_frames[0].save(outname, true);
      } else { printf("Could not open video:\n");}
      */
      // set window size to image size
      imgW = img.width();
      imgH = img.height();
      // img = img.resize((float)imgW/(float)imgH*MAX_WINDOW, MAX_WINDOW);
      // imgW = img.width();
      // imgH = img.height();
      setWindowSize(((float)imgW/(float)imgH)*500, 500); // resize fo rspeedup in image.cpp?
      // setWindowSize(imgW,imgH);

      eyePos = vec3(0, 0, 1);
      lookPos = vec3(0, 0, 0);
      upDir = vec3(0, 1, 0);
      // shaders = {"texture"};
      shaders = {"gaussian_dir","tensor","gaussian","histEqualize","boxblur","sobel","texture","quant","hatch"};
      // shaders = GetFilenamesInDir("../shaders", ".vs");

      //rotate 90, flip horrizontal, flip verticle, add, subtract, mulitply, crosshatch, lightest,darkest,sobel,tensor,box blur,quant,gaussian,directedgaussian (x2),dog,theshold,vingette, alphablend (x2), invert, grayscale, toTile, rnoise, crand
      for (string s : shaders) {
         renderer.loadShader(s, "../shaders/" + s + ".vs", "../shaders/" + s + ".fs");
      }
      // load images
      // textures = GetFilenamesInDir("../textures", "png");
      // for (int i = 0; i < textures.size(); i++){
      //    renderer.loadTexture(textures[i], "../textures/" + textures[i], ++ind);
      // } 
      
      
      // load crosshatching textures
      // hatch = GetFilenamesInDir("../textures/hatch", "png");
      // for (int i = 0; i < hatch.size(); i++){
      //    renderer.loadTexture(hatch[i], "../textures/hatch" + hatch[i], ++ind);
      //    cout << i << hatch[i]<<"\n";
      // } 
      hatch = {"pencil1","pencil2","ink1","ink2"};
      renderer.loadTexture(hatch[0], "../textures/hatch/" + hatch[0]+".png", 2);
      renderer.loadTexture(hatch[1], "../textures/hatch/" + hatch[1]+".png", 3);

      // // load brush strokes
      // brush = GetFilenamesInDir("../textures/brush", "png");
      // for (int i = 0; i < brush.size(); i++){
      //    renderer.loadTexture(brush[i], "../textures/brush" + brush[i], ++ind);
      // } 

      // setup for hist normalization
      cdf = getCDF(img);
      renderer.loadTexture("cdf", cdf, ind++);

      // set up for gaussian kernels
      float sigma = std::min(imgW,imgH)/150.0f;
      float k = 1.2;
      float tau  = 21;
      float cutoff = 190;
      g = gaussian(sigma);
      renderer.loadTexture("g", g.kernel, ind++);
      gk = gaussian(sigma*k);
      renderer.loadTexture("gk", gk.kernel, ind++);
      gEdge = gaussian(1.5f);
      renderer.loadTexture("gEdge", gEdge.kernel, ind++);

      //particle setup

      renderer.loadRenderTexture("paint_tensor", ind++, width(), height());
      renderer.loadTexture("particle", "../textures/brush/pencil_line.png", ind++);
      createConfetti(imgW,imgH);
      glEnable(GL_DEPTH_TEST);
      renderer.blendMode(agl::BLEND);

      //
   }

   //build gaussian kernel
   struct kernel gaussian(float sigma) {

      //build 1d gaussian kernel
      std::vector<float> k;

      float frac = (sqrt(2*M_PI)*sigma);
      int size = ceil(sigma*6);
      if (size>MAX_G){size=MAX_G;}

      // if (size <3){size=3;} else if (size>20){size =20;}
      int half = floor(size/2);

      // construct 1d gaussian vector
      for (int i = -1*half; i <= half; i++) {
         float val = (pow(M_E,-1*(pow(i,2))/(2*pow(sigma,2))));
         k.push_back(val/frac);
      }
      return kernel{size,frac,k};
   }

   // make cdf for image histogram normalization
   vector<float> getCDF(Image img) {
      unsigned char bins = 255;
      unsigned char min_ = 0;
      std::array<int, 256> pdf = {};
      vector<float>  cdf = {};
      for(int i=0; i<256; i++){
         pdf[i]=0;
         cdf.push_back(0.0f);
      }

      for(int i=0; i<img.height(); i++){
         for(int j=0; j<img.width(); j++){
            Pixel col = img.get(i,j);
            unsigned char c = .11*col.r + .59*col.g + .11*col.b;
            pdf[c%255]+=1;
         }
      }

      int sum = 0;
      bool toset = true;
      for(int i=0; i<256; i++){
         sum += pdf[i];
         if (toset && sum>0){
            toset = false;
            min_ = i;
         }
         cdf[i] = sum/(float)(img.width()*img.height());
      }
      cdf[255] = 1.0f;
      min = min_;
      return cdf;
   }
   //old
   Image CDFtoIMG(vector<float>  cdf) {
      Image result = Image(256,1);
      for(int i=0; i<256; i++){
         int large = cdf[i];
         unsigned char r = large>>24;
         large = large - (r<<24);
         unsigned char g = large>>16;
         large = large - (g<<16);
         unsigned char b = large>>8;
         large = large - (b<<8);
         unsigned char a = large;
         result.set(0,i,Pixel{r,g,b,a});

      }
      //convert back:
      // unsigned int large = (r<<24)|(g<<16)|(b<<8)|(a)
   }

 void createConfetti(int w, int h) {
    for (int i = 0; i < h; i++){
      for (int j = 0; j < w; j++){
         Particle particle;
         Pixel col = img.get(i,j);
         particle.color = vec4(col.r/255.0f,col.g/255.0f,col.b/255.0f,col.a/255.0f);
         particle.size = 0.1;// det in rendering pipeline/after tensor
         particle.rot = 0.0;
         particle.pos = vec3(i/w,j/w,1.5);
         mParticles.push_back(particle);
         }
      }
  }

  void updateConfetti()
  {
   for (int i = 0; i < imgH; i++){
      for (int j = 0; i < imgW; i++){
      Particle particle = mParticles[i*imgW+j];
      // ...
      mParticles[i*imgW+j] = particle;
      }}
  }

  void drawConfetti()  {
   renderer.beginShader("sprite");
      vec3 cameraPos = renderer.cameraPosition();
      float aspect = ((float)width()) / height();
      renderer.ortho(0, aspect , 0, aspect , 1.0f, 2.0f);

      renderer.lookAt(eyePos, lookPos, upDir);
      // updateConfetti();


    // draw
    renderer.texture("image", "particle");
    renderer.texture("tensor", "paint_tensor");
    for (int i = 0; i < mParticles.size(); i++)
    {
      Particle particle = mParticles[i];
      renderer.sprite(particle.pos, particle.color, particle.size, particle.rot);
    }

    renderer.endShader();
  }

   void mouseMotion(int x, int y, int dx, int dy) {
      if (mouseIsDown(GLFW_MOUSE_BUTTON_LEFT)) {
      }
   }

   void mouseDown(int button, int mods){
   }

   void scroll(float dx, float dy) {
   }

   void keyUp(int key, int mods) {
      if (key ==  GLFW_KEY_RIGHT) {
         shaderIDX = (shaderIDX +1)%(shaders.size());
      }
      if (key ==  GLFW_KEY_LEFT) {
         shaderIDX = (shaderIDX -1)%(shaders.size());
      }
   }

   // void update(){}
   
   // final operations (memory cleaning and such)
   void fnExit(){img.clear();}


   void draw() {
      // print fps
      time1 = (double) clock() / CLOCKS_PER_SEC;  
      frames++;
      
      if(abs(time0 - time1) > 8){
         cout<< "fps: "<<frames/8.0<<"\n";
         time0 = time1;
         frames = 0;
      }
      // update();
      float aspect = ((float)width()) / height();// window aspect ratio

      // painterly PIPELINE
      /**/
      renderer.beginRenderTexture("paint_tensor");
         // --- INIT SHADER
         renderer.beginShader("tensor"); // activates shader with given name
         // set viewer
         renderer.lookAt(eyePos,lookPos,upDir);
         renderer.ortho(0, aspect , 0, aspect , 1.0f, 2.0f);

         // image 
         renderer.texture("Image", textures[0]);
         renderer.texture("hatch1", hatch[0]);
         renderer.texture("hatch2", hatch[1]);
         renderer.setUniform("imgW", imgW);
         renderer.setUniform("imgH", imgH);
         renderer.setUniform("sobelx",sobelx);
         renderer.setUniform("sobely",sobely);
         renderer.setUniform("edgeAligned",true);

         // scale quad
         renderer.scale(vec3(aspect, aspect, 1.0f));
         // move quad
         // renderer.translate(vec3(-1, -1, 0));
         // make quad
         renderer.quad();
         

         background(vec3(0));

         renderer.endShader();
      renderer.endRenderTexture();
      //after tensor is put into "tensor texture" draw particles
      drawConfetti();
   
/*
      // --- INIT SHADER - crosshatch, sobel, blur, color quant
      renderer.beginShader(shaders[shaderIDX]); // activates shader with given name
      // set viewer
      renderer.lookAt(eyePos,lookPos,upDir);
      renderer.ortho(0, aspect , 0, aspect , 1.0f, 2.0f);

      // image 
      renderer.texture("Image", textures[0]);
      renderer.texture("hatch1", hatch[0]);
      renderer.texture("hatch2", hatch[1]);

      // scale quad
      renderer.scale(vec3(aspect, aspect, 1.0f));
      // move quad
      // renderer.translate(vec3(-1, -1, 0));
      // make quad
      renderer.quad();
      renderer.setUniform("imgW", imgW);
      renderer.setUniform("imgH", imgH);
      renderer.setUniform("sobelx",sobelx);
      renderer.setUniform("sobely",sobely);
      renderer.setUniform("min",min);
      renderer.setUniform("size",g.size);
      renderer.setUniform("denom",g.denom);
      renderer.texture("cdf","cdf");
      renderer.texture("g","g");


      // ---PASS PARAMETERS to shaders
      // renderer.setUniform("name", var);

      
      renderer.endShader();*/
      // renderer.beginRenderTexture("tensor");

   
/*
   // XDoG PIPELINE

   // render quad with image texture (1)
   // save render to a named texture (instead of screen)
   // call named texture a new quad texture
   // layer shaders (2)
   // render pass #1
   // save first gaussian blur to texture named g
   renderer.beginRenderTexture("g");
      renderer.beginShader(shaders[1]); // activates shader with given name
      // set viewer
      renderer.lookAt(eyePos,lookPos,upDir);
      renderer.ortho(0, aspect , 0, aspect , 1.0f, 2.0f);

      // image 
      renderer.texture("Image", textures[0]);
      renderer.texture("hatch1", hatch[0]);
      renderer.texture("hatch2", hatch[1]);

      // scale quad
      renderer.scale(vec3(aspect, aspect, 1.0f));
      // move quad
      // renderer.translate(vec3(-1, -1, 0));
      // make quad
      renderer.quad();
      renderer.setUniform("imgW", imgW);
      renderer.setUniform("imgH", imgH);
      renderer.setUniform("sobelx",sobelx);
      renderer.setUniform("sobely",sobely);

      background(vec3(0));

      renderer.endShader();
   renderer.endRenderTexture();

    // render pass #2
   // save first gaussian blur to texture named gk
   renderer.beginRenderTexture("gk");
      // --- INIT SHADER
      renderer.beginShader(shaders[1]); // activates shader with given name
      // set viewer
      renderer.lookAt(eyePos,lookPos,upDir);
      renderer.ortho(0, aspect , 0, aspect , 1.0f, 2.0f);

      // image 
      renderer.texture("Image", textures[0]);
      renderer.texture("hatch1", hatch[0]);
      renderer.texture("hatch2", hatch[1]);

      // scale quad
      renderer.scale(vec3(aspect, aspect, 1.0f));
      // move quad
      // renderer.translate(vec3(-1, -1, 0));
      // make quad
      renderer.quad();
      renderer.setUniform("imgW", imgW);
      renderer.setUniform("imgH", imgH);
      renderer.setUniform("sobelx",sobelx);
      renderer.setUniform("sobely",sobely);

      background(vec3(0));

      renderer.endShader();
   renderer.endRenderTexture();

   //use previostextures g and gk as texture inputs to dog 

    renderer.beginShader("screeneffects");
    lookAt(vec3(0), vec3(0, 0, -2));
    ortho(0, width(), 0, height(), -1, 1);
    background(vec3(0));
    renderer.setUniform("EffectType", effect);
    renderer.setUniform("GlobalTime", elapsedTime());
    renderer.setUniform("Resolution", vec2(width(), height()));
    renderer.texture("g", "g");
    renderer.texture("gk", "gk");
    renderer.identity();
    renderer.translate(vec3(width()*0.5, height()*0.5, 0.0));
    renderer.scale(vec3(width(), height(), 1.0f));
    renderer.rotate(kPI / 2, vec3(1, 0, 0));
    renderer.plane();
    renderer.endShader();*/
   }




protected:
   vec3 eyePos;
   vec3 lookPos;
   vec3 upDir;
   std::vector<string> textures;
   std::vector<string> videos;
   std::vector<agl::Image> video_frames;

   std::vector<string> shaders;
   unsigned int shaderIDX = 0;
   std::vector<string> hatch;
   std::vector<string> brush;
   int imgW;
   int imgH;
   Image img;
   mat3 sobelx = mat3(
      -1,-2,-1,
      0,0,0,
      1,2,1
   );
   mat3 sobely = mat3(
      -1,0,1,
      -2,0,2,
      -1,0,1
   );
   unsigned char min = 0;
   // std::array<long,256> cdf = {};
   vector<float> cdf;

   // brush stroke particles
   std::vector<Particle> mParticles;
   int frames;
   struct kernel g;
   struct kernel gk;
   struct kernel gEdge;
   double time0 = 0.0;
   double time1 = 0.0;
};




int main(int argc, char **argv) {
   // atexit (fnExit);

   Viewer viewer;
   viewer.run();
   return 0;
}
