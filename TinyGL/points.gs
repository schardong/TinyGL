#version 330

layout (points) in;
layout (points, max_vertices = 1) out;

in VertexAttrib
{
  vec4 color;
  vec4 normal;
} in_vData[];

out Attrib
{
  vec4 color;
  vec4 normal;
} out_gData;

void main()
{
  int i;
  for(int i = 0; i < gl_in.length(); i++) {
    gl_Position = gl_in[i].gl_Position;
    out_gData.color = in_vData[i].color;
    out_gData.normal = in_vData[i].normal;
    EmitVertex();
  }
}