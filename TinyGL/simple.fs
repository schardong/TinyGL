#version 330

in Attrib
{
  vec4 color;
  vec4 normal;
} in_gData;

layout (location = 0) out vec4 out_vColor;

void main()
{
  out_vColor = in_gData.color;
}