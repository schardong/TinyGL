#version 330 core

layout (location = 0) in vec3 in_vPosition;
layout (location = 1) in vec2 in_vTexCoord;

uniform mat4 modelMatrix;
uniform mat4 projMatrix;

out vec2 vTexCoord;
out vec3 vLightPos[4];

void main()
{
  mat4 MP = projMatrix * modelMatrix;
  vTexCoord = in_vTexCoord;
  vLightPos[0] = vec3(0, 6, 0);
  vLightPos[1] = vec3(20, 6, 0);
  vLightPos[2] = vec3(20, 6, 20);
  vLightPos[3] = vec3(0, 6, 20);
  gl_Position = MP * vec4(in_vPosition, 1.0);
}
