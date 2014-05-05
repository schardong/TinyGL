#version 330 core

layout (location = 0) out vec4 fColor;

uniform sampler2D u_diffuseMap;
uniform sampler2D u_normalMap;
uniform sampler2D u_vertexMap;
uniform mat4 viewMatrix;

const int g_numLights = 3;
vec4 g_ambientColor = vec4(0.1);

in vec2 vTexCoord;

struct LightParam
{
  vec3 pos;
  vec3 color;
};

layout (std140) uniform LightSource
{
  LightParam lights[g_numLights];
};

void main()
{
  vec3 normal_camera = normalize(texture(u_normalMap, vTexCoord)).xyz;
  if(length(normal_camera) == 0)
    fColor = vec4(0.8);
  else {
    fColor = vec4(0.f);
    vec4 diff_color = texture(u_diffuseMap, vTexCoord);
    vec3 vertex_camera = (texture(u_vertexMap, vTexCoord)).xyz;
	
    for(int i = 0; i < g_numLights; i++) {
      vec3 light_camera = (viewMatrix * vec4(lights[i].pos, 1)).xyz;
      vec3 light_dir = light_camera - vertex_camera;
      float dist = length(light_dir);
      light_dir = normalize(light_dir);
      
      float diff = max(dot(normal_camera, light_dir), 0.f);
      fColor += (diff * diff_color * vec4(lights[i].color, 1)) / (dist * dist);
      
      vec3 R = reflect(-light_dir, normal_camera);
      //float spec = max(dot(normalize(-vertex_camera), R), 0.f); // (dist * dist);
      //fColor = vec4(pow(spec, 2.f));
    }
    //fColor = vec4(-vertex_camera, 1);
    fColor += g_ambientColor;
  }
  
}
