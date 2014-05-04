#version 330 core

layout (location = 0) in vec3 in_vPosition;
layout (location = 1) in vec2 in_vTexCoord;

uniform mat4 modelMatrix;
uniform mat4 projMatrix;
vec3 g_lightPos = vec3(0, 6, 4);

out vec2 vTexCoord;
out vec3 vLightPos;

void main()
{
  mat4 MP = projMatrix * modelMatrix;
  vTexCoord = in_vTexCoord;
  vLightPos = g_lightPos;
  gl_Position = MP * vec4(in_vPosition, 1.0);
}
