#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VertexAttrib
{
  vec4 coord;
  vec4 color;
  smooth vec4 normal;
} in_vData[];

in Light
{
  vec4 eye;
  vec4 light;
} in_vLight[];

out VertexAttrib
{
  vec4 coord;
  vec4 color;
  smooth vec4 normal;
} out_gData;

out Light
{
  vec4 eye;
  vec4 light;
} out_gLight;

void main()
{
  for(int i = 0; i < gl_in.length(); i++) {
    gl_Position = gl_in[i].gl_Position;
    out_gData.coord = in_vData[i].coord;
    out_gData.color = in_vData[i].color;
    out_gData.normal = in_vData[i].normal;

    out_gLight.eye = in_vLight[i].eye;
    out_gLight.light = in_vLight[i].light;

    EmitVertex();
  }

  EndPrimitive();
}