#version 330 core

layout (location = 0) in vec3 in_vPosition;
layout (location = 1) in vec3 in_vColor;
layout (location = 2) in vec3 in_vNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat3 normalMatrix;

uniform vec3 u_eyeCoord;
uniform vec3 u_lightCoord;

out VertexAttrib
{
  vec4 coord;
  vec4 color;
  smooth vec4 normal;
} out_vData;

out Light
{
  vec4 eye;
  vec4 light;
} out_vLight;

void main()
{
  mat4 MV = viewMatrix * modelMatrix;
  mat4 MVP = projMatrix * MV;

  gl_Position = MVP * vec4(in_vPosition, 1.0);

  out_vData.coord = (modelMatrix) * vec4(in_vPosition, 1.0);
  out_vData.color = vec4(in_vColor, 1.0);
  out_vData.normal = vec4(normalMatrix * in_vNormal, 0);

  out_vLight.eye = vec4(u_eyeCoord, 1);
  out_vLight.light = vec4(u_lightCoord, 1);
}