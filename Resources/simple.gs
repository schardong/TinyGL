#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VertexAttrib
{
  vec4 coord_camera;
  vec4 color;
  smooth vec4 normal_camera;
} in_vData[];

in Light
{
  vec4 eye;
  vec4 light_camera;
} in_vLight[];

out VertexAttrib
{
  vec4 coord_camera;
  vec4 color;
  smooth vec4 normal_camera;
} out_gData;

out Light
{
  vec4 eye;
  vec4 light_camera;
} out_gLight;

void main()
{
  for(int i = 0; i < gl_in.length(); i++) {
    gl_Position = gl_in[i].gl_Position;
    out_gData.coord_camera = in_vData[i].coord_camera;
    out_gData.color = in_vData[i].color;
    out_gData.normal_camera = in_vData[i].normal_camera;

    out_gLight.eye = in_vLight[i].eye;
    out_gLight.light_camera = in_vLight[i].light_camera;

    EmitVertex();
  }

  EndPrimitive();
}