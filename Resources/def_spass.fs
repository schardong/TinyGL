#version 330 core

layout (location = 0) out vec4 fColor;

uniform sampler2D u_diffuseMap;
uniform sampler2D u_normalMap;
uniform sampler2D u_vertexMap;

/*

in LightData
{
  vec3 vertex_camera;
  vec3 lightDir_camera;
  vec3 normal_camera;
} in_vLight;

vec4 g_ambientColor = vec4(0.1);*/
vec4 g_ambientColor = vec4(0.1);

in vec2 vTexCoord;

vec4 light_dir = vec4(0, 1, 0, 0);

void main()
{
  vec4 diff_color = texture(u_diffuseMap, vTexCoord);
  vec4 normal_camera = texture(u_normalMap, vTexCoord);
  vec4 vertex_camera = texture(u_vertexMap, vTexCoord);
  
  float diff = max(dot(normal_camera, light_dir), 0.f);
  fColor = diff * diff_color + g_ambientColor;
  
  /*
  float diff = max(dot(in_vLight.normal_camera, in_vLight.lightDir_camera), 0.f);
  diffColor = diff * u_materialColor + g_ambientColor;

  vec3 reflection = normalize(reflect(-in_vLight.lightDir_camera, in_vLight.normal_camera));
  float spec = max(dot(normalize(in_vLight.vertex_camera), reflection), 0.f);
  if(diff != 0) {
    spec = pow(spec, 64.f);
    out_vColor.rgb += vec3(spec);
  }*/

}