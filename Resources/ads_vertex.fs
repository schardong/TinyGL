#version 330 core

in vec4 vColor;

layout (location = 0) out vec4 out_vColor;

void main()
{
  out_vColor = vColor;
}