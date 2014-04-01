#version 330 core

uniform vec4 u_materialColor;

layout (location = 0) out vec4 out_vColor;

in LightData
{
  vec3 normal_camera;
  vec3 lightDir_camera;
} in_vLight;

vec4 g_ambientColor = vec4(0.1);

void main()
{
  float diff = max(dot(in_vLight.normal_camera, in_vLight.lightDir_camera), 0.f);
  out_vColor = diff * u_materialColor + g_ambientColor;
  
  vec3 reflection = normalize(reflect(-in_vLight.lightDir_camera, in_vLight.normal_camera));
  float spec = max(dot(normalize(in_vLight.normal_camera), reflection), 0.f);
  if(diff != 0) {
    spec = pow(spec, 128.f);
    out_vColor.rgb += vec3(spec);
  }

}