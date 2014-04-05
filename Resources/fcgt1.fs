#version 330 core

layout (location = 0) out vec4 out_fColor;

in vec4 out_vColor;

void main()
{
  out_fColor = out_vColor;
}
