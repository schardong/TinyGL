#version 330

layout (triangles) in;
layout (points, max_vertices = 3) out;

in vec4 ex_vColor[];
out vec4 ex_gColor[];

void main()
{
  for(int i = 0; i < gl_in.length(); i++) {
    gl_Position = gl_in[i].gl_Position;
    ex_gColor[i] = ex_vColor[i];
    EmitVertex();
  }

  EndPrimitive();
}