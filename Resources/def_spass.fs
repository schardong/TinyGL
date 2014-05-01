#version 330 core

layout (location = 0) out vec4 fColor;

uniform sampler2D u_diffuseMap;
uniform sampler2D u_normalMap;
uniform sampler2D u_vertexMap;
uniform samplerBuffer u_lightCoords;

uniform mat4 viewMatrix;

vec4 g_ambientColor = vec4(0.1);
vec4 g_lightPos = vec4(10, 20, 10, 1);

in vec2 vTexCoord;

void main()
{
  vec4 diff_color = texture(u_diffuseMap, vTexCoord);
  if(length(diff_color) == 0) discard;
  
  vec4 normal_camera = texture(u_normalMap, vTexCoord);
  vec4 vertex_camera = texture(u_vertexMap, vTexCoord);
  vec4 light_world;
  
  light_world = texelFetch(u_lightCoords, 0);
  /*for(int i = 0; i < 50; i++) {
    light_world[i].x = texelFetch(u_lightCoords, i * 3);
    light_world[i].y = texelFetch(u_lightCoords, i * 3 + 1);
    light_world[i].z = texelFetch(u_lightCoords, i * 3 + 2);
  }*/
  fColor = light_world;
  
  /*vec4 light_dir = normalize((viewMatrix * g_lightPos) - vertex_camera);
  
  float diff = max(dot(normal_camera, light_dir), 0.f);
  fColor = diff * diff_color + g_ambientColor;
  
  vec4 reflection = normalize(reflect(-light_dir, normal_camera));
  float spec = max(dot(normalize(vertex_camera), reflection), 0.f);
  
  if(diff != 0) {
    spec = pow(spec, 128.f);
    fColor.rgb += vec3(spec);
  }*/

}