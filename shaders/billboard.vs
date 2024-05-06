#version 400

layout (location = 0) in vec3 vPosition;

uniform vec3 CameraPos;
uniform float Size;
uniform float Rot;
uniform sampler2D tensor;
uniform vec3 Offset;
uniform vec4 Color;
uniform mat4 MVP;

out vec4 color;
out vec2 uv;

void main()
{
  float xdir = texture(tensor,uv).x*2.0f - 1;
  float ydir = texture(tensor,uv).y*2.0f - 1;
  vec3 offset = vec3(Offset.xy, texture(tensor,uv).z);
  // Rot = atan(ydir/xdir);

  color = Color;
  uv = vPosition.xy;

  vec3 z = normalize(CameraPos - offset);
  vec3 x = normalize(cross(vec3(0,1,0), z));
  vec3 y = normalize(cross(z, x));
  mat3 R = mat3(x, y, z);

  x = vec3(xdir, -ydir, 0);
  y = vec3(xdir, xdir, 0);
  // x = vec3(cos(Rot), -sin(Rot), 0);
  // y = vec3(sin(Rot), cos(Rot), 0);
  z = vec3(0,0,1);
  mat3 M = mat3(x, y, z);

  vec3 eyePos = M * R * Size * (vPosition - vec3(0.5, 0.5, 0.0)) + offset;
  gl_Position = MVP * vec4(eyePos, 1.0); 
}
