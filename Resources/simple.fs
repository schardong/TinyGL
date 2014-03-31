#version 330 core

in VertexAttrib
{
  vec4 coord_camera;
  vec4 color;
  smooth vec4 normal_camera;
} in_gData;

in Light
{
  vec4 eye;
  vec4 light_camera;
} in_gLight;

layout (location = 0) out vec4 out_vColor;

float g_ambientColor = 0.1;
vec4 light_color = vec4(1.0, 1.0, 1.0, 1.0);

void main()
{
  float distance = length(in_gLight.light_camera - in_gData.coord_camera);
  vec4 lightDir = normalize(in_gLight.light_camera - in_gData.coord_camera);
  float intensity = max(dot(lightDir, in_gData.normal_camera), 0.0);

  vec4 diffuseColor = intensity * light_color * in_gData.color * 20 / (distance * distance);
  vec4 specularColor = vec4(0.f);

  vec4 reflection = normalize(reflect(-lightDir, normalize(in_gData.normal_camera)));

  float spec = max(dot(in_gData.normal_camera, reflection), 0.0);
  if(intensity != 0) {
    spec = pow(spec, 5.f);
    specularColor = vec4(spec, spec, spec, 0) * light_color / (distance * distance);
  }

  out_vColor = g_ambientColor + diffuseColor + specularColor;
}