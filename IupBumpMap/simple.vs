#version 330

layout (location = 0) in vec3 in_vPosition;
layout (location = 1) in vec3 in_vColor;
layout (location = 2) in vec3 in_vNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 normalMatrix;

out VertexAttrib
{
  vec4 color;
  vec4 normal;
} out_vData;

void main()
{
  mat4 MVP = projMatrix * viewMatrix * modelMatrix;
  gl_Position = MVP * vec4(in_vPosition, 1.0);
  out_vData.color = vec4(in_vColor, 1.0);
  out_vData.normal = normalMatrix * vec4(in_vNormal, 0.0);
}