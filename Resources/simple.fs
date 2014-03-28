#version 330

in Attrib
{
  vec4 color;
  vec4 normal;
} in_gData;

layout (location = 0) out vec4 out_vColor;

vec4 light_pos = vec4(10, 10, 5, 1);
vec4 light_color = vec4(1.0, 1.0, 1.0, 1.0);

void main()
{
  vec4 lightDir = normalize(light_pos);
  float intensity = max(dot(lightDir, in_gData.normal), 0.0);

  vec4 diffuseColor = intensity * light_color * in_gData.color;
  vec4 specularColor = pow(intensity, 5) * light_color * vec4(1.0);
  out_vColor =  diffuseColor + specularColor;
}