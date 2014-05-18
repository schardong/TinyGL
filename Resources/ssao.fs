#version 330 core

layout (location = 0) out vec4 fColor;

uniform sampler2D u_diffuseMap;
uniform sampler2D u_normalMap;
uniform sampler2D u_vertexMap;
uniform sampler2D u_depthMap;

uniform mat4 viewMatrix;
uniform int u_numLights;
uniform float u_zNear;
uniform float u_zFar;

const int g_maxLights = 50;
vec4 g_ambientColor = vec4(0.1);

in vec2 vTexCoord;

layout (std140) uniform LightPos
{
  vec4 u_lightPos[g_maxLights];
};

float linearizeDepth(vec2 uv)
{
  float n = u_zNear; // camera z near
  float f = u_zFar;  // camera z far
  float z = texture(u_depthMap, uv).x; 
  return (2.0 * n) / (f + n - z * (f - n));
}

void main()
{
  vec3 normal_camera = (texture(u_normalMap, vTexCoord)).xyz;
  if(length(normal_camera) == 0)
    fColor = vec4(0.8);
  else {
    if(u_numLights > g_maxLights) discard;
    fColor = vec4(0.f);
    vec4 diff_color = texture(u_diffuseMap, vTexCoord);
    vec3 vertex_camera = (texture(u_vertexMap, vTexCoord)).xyz;
	
    for(int i = 0; i < u_numLights; i++) {
      vec3 light_camera = (viewMatrix * u_lightPos[i]).xyz;
      vec3 light_dir = light_camera - vertex_camera;
      float dist = length(light_dir);
      light_dir = normalize(light_dir);
      
      float diff = max(dot(normal_camera, light_dir), 0.f);
      fColor.rgb += diff * diff_color.rgb / u_numLights;
      
      if(diff > 0.f) {
        vec3 V = normalize(-vertex_camera);
        vec3 R = normalize(reflect(-light_dir, normal_camera));
        
        float angle = max(dot(R, V), 0.f);
        float spec = pow(angle, 128.f);
        vec3 specColor = vec3(spec);
        fColor.rgb += specColor;
      }
    }
    fColor += g_ambientColor;
  }
  //float d = linearizeDepth(vTexCoord);
  //fColor = vec4(d, d, d, 1.0);
}
