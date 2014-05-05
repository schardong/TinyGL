#version 330 core

layout (location = 0) in vec3 in_vPosition;
layout (location = 1) in vec2 in_vTexCoord;

uniform mat4 modelMatrix;
uniform mat4 projMatrix;

out vec2 vTexCoord;
out vec3 vLightPos[5];

struct LightParameters
{
  vec3 pos;
  vec3 color;
};

void main()
{
  mat4 MP = projMatrix * modelMatrix;
  vTexCoord = in_vTexCoord;
  vLightPos[0] = vec3(0, 10, 0);
  vLightPos[1] = vec3(20, 10, 0);
  vLightPos[2] = vec3(20, 10, 20);
  vLightPos[3] = vec3(0, 10, 20);
  vLightPos[4] = vec3(10, 2, 10);
  gl_Position = MP * vec4(in_vPosition, 1.0);
}
