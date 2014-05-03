#version 330 core

layout (location = 0) in vec3 in_vPosition;
layout (location = 1) in vec2 in_vTexCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec2 vTexCoord;

void main()
{
  mat4 MVP = projMatrix * mat4(1) * modelMatrix;
  vTexCoord = in_vTexCoord;
  gl_Position = MVP * vec4(in_vPosition, 1.0);
}
