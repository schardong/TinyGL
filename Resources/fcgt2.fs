#version 330 core

layout (location = 0) out vec4 fColor;

in vec2 vTexCoord;

void main()
{
  fColor = vec4(vTexCoord, 0, 1);
}
