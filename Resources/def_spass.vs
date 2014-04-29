#version 330 core

layout (location = 0) in vec3 in_vPosition;
/*layout (location = 1) in vec3 in_vNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat3 normalMatrix;

uniform vec3 u_eyeCoord;
uniform vec3 u_lightCoord;
uniform vec4 u_materialColor;

out LightData
{
  vec3 vertex_camera;
  vec3 lightDir_camera;
  vec3 normal_camera;
} out_vLight;*/

void main()
{
  /*mat4 MV = viewMatrix * modelMatrix;
  mat4 MVP = projMatrix * MV;
  
  vec4 pos4 = MV * vec4(in_vPosition, 1);
  vec3 pos3 = pos4.xyz / pos4.w;

  out_vLight.vertex_camera = normalize(-pos3);
  out_vLight.normal_camera = normalize(normalMatrix * in_vNormal);
  out_vLight.lightDir_camera = normalize((viewMatrix * vec4(u_lightCoord, 1)).xyz - pos3);*/
  
  gl_Position = vec4(in_vPosition, 1.0);
}