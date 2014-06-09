#version 330 core

layout (location = 0) in vec3 in_vPosition;
layout (location = 1) in vec3 in_vColor;

uniform mat4 MVP;

out vec4 vColor;

void main()
{
  vColor = vec4(in_vColor, 1.f);
  gl_Position = MVP * vec4(in_vPosition, 1.f);
}
