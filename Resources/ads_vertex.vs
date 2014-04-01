#version 330 core

layout (location = 0) in vec3 in_vPosition;
layout (location = 1) in vec3 in_vNormal;

smooth out vec4 vColor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat3 normalMatrix;

uniform vec3 u_eyeCoord;
uniform vec3 u_lightCoord;
uniform vec4 u_materialColor;

float g_ambientColor = vec4(0.1);
//vec4 g_lightColor = vec4(1.0, 1.0, 1.0, 1.0);
//float g_lightPower = 55;

void main()
{
  mat4 MV = viewMatrix * modelMatrix;
  mat4 MVP = projMatrix * MV;
  
  vec3 normalCamera = normalize(normalMatrix * in_vNormal);
  vec4 pos4 = MV * vec4(in_vPosition, 1);
  vec3 pos3 = pos4.xyz / pos4.w;
  
  vec3 lightCamera = normalize((viewMatrix * vec4(u_lightCoord, 1)).xyz - pos3);
  float diff = max(dot(normalCamera, lightCamera), 0.f);
  vColor = diff * u_materialColor + g_ambientColor;
  
  vec3 reflection = normalize(reflect(-lightCamera, normalCamera));
  float spec = max(dot(normalCamera, reflection), 0.f);
  if(diff != 0) {
    spec = pow(spec, 128.f);
    vColor.rgb += vec3(spec);
  }

  gl_Position = MVP * vec4(in_vPosition, 1.0);
}