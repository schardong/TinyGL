#version 330 core

layout (location = 0) in vec3 in_vPosition;
layout (location = 1) in vec3 in_vNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat3 normalMatrix;

out LightData
{
  vec3 vertex_camera;
  vec3 normal_camera;
} vLight;

void main()
{
  mat4 MVP = projMatrix * viewMatrix * modelMatrix;
  mat4 MV = viewMatrix * modelMatrix;

  vec4 pos4 = MV * vec4(in_vPosition, 1);
  vec3 pos3 = pos4.xyz / pos4.w;

  vLight.vertex_camera = pos3;
  vLight.normal_camera = normalMatrix * in_vNormal;
  
  gl_Position = MVP * vec4(in_vPosition, 1.0);
}
