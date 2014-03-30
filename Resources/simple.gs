#version 330

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VertexAttrib
{
  vec4 color;
  vec4 normal;
} in_vData[];

out VertexAttrib
{
  vec4 color;
  vec4 normal;
} out_gData;

void main()
{
  vec3 n1 = normalize(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz);
  vec3 n2 = normalize(gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz);
  vec4 faceNormal = vec4(cross(n2, n1), 0);

  for(int i = 0; i < gl_in.length(); i++) {
    gl_Position = gl_in[i].gl_Position;
    out_gData.color = in_vData[i].color;
    out_gData.normal = in_vData[i].normal;
    //out_gData.normal = faceNormal;
    EmitVertex();
  }

  EndPrimitive();
}