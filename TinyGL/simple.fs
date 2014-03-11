#version 330

in VertexAttrib
{
  vec4 color;
  vec4 normal;
} in_vData;

layout (location = 0) out vec4 out_vColor;

void main()
{
  out_vColor = in_vData.color;
}