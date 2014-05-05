#version 330 core

layout (location = 0) out vec4 fColor;

uniform sampler2D u_diffuseMap;
uniform sampler2D u_normalMap;
uniform sampler2D u_vertexMap;
uniform mat4 viewMatrix;

vec4 g_ambientColor = vec4(0.1);

in vec2 vTexCoord;
in vec3 vLightPos[5];

void main()
{
  vec3 normal_camera = normalize(texture(u_normalMap, vTexCoord)).xyz;
  if(length(normal_camera) == 0)
    fColor = vec4(0.8);
  else {
    fColor = vec4(0.f);
    vec4 diff_color = texture(u_diffuseMap, vTexCoord);
    vec3 vertex_camera = (texture(u_vertexMap, vTexCoord)).xyz;
	
    for(int i = 0; i < 5; i++) {
      vec3 light_camera = (viewMatrix * vec4(vLightPos[i], 1)).xyz;
      vec3 light_dir = normalize(light_camera - vertex_camera);
      float diff = max(dot(normal_camera, light_dir), 0.f);
      fColor += (diff * diff_color + g_ambientColor) / 2;
      
      //vec3 R = reflect(-light_dir, normal_camera);
      //float spec = max(dot(normalize(-vertex_camera), R), 0.f) / 10;
      //fColor += vec4(pow(spec, 3.f));
    }
  }
}
