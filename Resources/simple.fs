#version 330 core

in VertexAttrib
{
  vec4 coord;
  vec4 color;
  smooth vec4 normal;
} in_gData;

in Light
{
  vec4 eye;
  vec4 light;
} in_gLight;

layout (location = 0) out vec4 out_vColor;

vec4 light_pos = vec4(0, 5, 0, 1);
vec4 light_color = vec4(1.0, 1.0, 1.0, 1.0);

void main()
{
  vec4 lightDir = normalize(in_gLight.light - in_gData.coord);
  float intensity = max(dot(lightDir, in_gData.normal), 0.0);

  vec4 diffuseColor = intensity * light_color * in_gData.color;
  vec4 specularColor = pow(intensity, 15) * light_color * vec4(1.0);
  out_vColor = in_gData.normal;
}