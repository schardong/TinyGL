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
in vec3 vLightPos;

void main()
{
  vec3 normal_camera = normalize(texture(u_normalMap, vTexCoord)).xyz;
  if(length(normal_camera) == 0)
    fColor = vec4(0.8);
  else {
    vec4 diff_color = texture(u_diffuseMap, vTexCoord);
    vec3 vertex_camera = (texture(u_vertexMap, vTexCoord)).xyz;
    vec3 light_world = vLightPos;//texelFetch(u_lightCoords, 20);
    
    vec3 light_dir = normalize((viewMatrix * vec4(light_world, 1)) - vec4(vertex_camera, 1)).xyz;
	
	fColor = vec4(vertex_camera, 1);

    /* float diff = max(dot(normal_camera, light_dir), 0.f);
    fColor = diff * diff_color + g_ambientColor;
    
    vec3 reflection = normalize(reflect(light_dir, normal_camera));
    float spec = max(dot(normalize(vertex_camera), reflection), 0.f);
	   
	if(diff != 0) {
      spec = pow(spec, 128.f);
      fColor.rgb += vec3(spec);
    } */
  }

}
