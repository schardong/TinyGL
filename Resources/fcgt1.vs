#version 330 core

layout (location = 0) in vec3 in_vPosition;
layout (location = 1) in vec3 in_vColor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec4 out_vColor;

void main()
{
  mat4 MVP = projMatrix * viewMatrix * modelMatrix;
  out_vColor = vec4(in_vColor, 1.f);
  gl_Position = MVP * vec4(in_vPosition, 1.f);
}
