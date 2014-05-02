#version 330 core

layout (location = 0) out vec4 diffColor;
layout (location = 1) out vec4 normalEye;
layout (location = 2) out vec4 vertexEye;

uniform vec4 u_materialColor;

in LightData
{
  vec3 vertex_world;
  vec3 normal_camera;
} vLight;

void main()
{
  diffColor = u_materialColor;
  normalEye = vec4(vLight.normal_camera, 1);
  vertexEye = vec4(vLight.vertex_world, 1);
}