#version 330

in vec4 ex_gColor;
layout (location = 0) out vec4 out_vColor;

void main()
{
  out_vColor = ex_gColor;
}