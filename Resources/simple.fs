#version 330

in Attrib
{
  vec4 color;
  vec4 normal;
} in_gData;

layout (location = 0) out vec4 out_vColor;

vec4 light_pos = vec4(0, 10, 0, 1);

void main()
{
  vec4 lightDir = normalize(light_pos);
  float intensity = max(dot(lightDir, in_gData.normal), 0.0);
  if(intensity > 0.98)
    out_vColor = vec4(1.0);
  else
    out_vColor =  intensity * in_gData.color;
}