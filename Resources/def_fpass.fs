#version 330 core

layout (location = 0) out vec3 diffColor;
layout (location = 1) out vec3 normalEye;
layout (location = 2) out vec3 vertexEye;

uniform vec4 u_materialColor;

in LightData
{
  vec3 vertex_camera;
  vec3 normal_camera;
} vLight;

void main()
{
  diffColor = u_materialColor.rgb;
  normalEye = normalize(vLight.normal_camera);
  vertexEye = vLight.vertex_camera;
}
