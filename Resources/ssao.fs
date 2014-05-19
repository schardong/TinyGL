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

uniform vec2 u_screenSize;

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

float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
  //vec3 kernel[16];
  vec3 normal_camera = (texture(u_normalMap, vTexCoord)).xyz;
  if(length(normal_camera) == 0)
    fColor = vec4(0.8);
  else {
    float occ_factor = 0;
    vec2 tex_step = vec2(1 / u_screenSize.x, 1 / u_screenSize.y);
    
    float aux = 5*tex_step.x;
    float auy = 5*tex_step.x;
    
    float my_depth = linearizeDepth(vTexCoord);
    for(float x = vTexCoord.x - aux; x < vTexCoord.x + aux; x += tex_step.x) {
      for(float y = vTexCoord.y - auy; y < vTexCoord.y + auy; y += tex_step.y) {
        float depth = linearizeDepth(vec2(x, y));
        occ_factor += my_depth - depth;
      }
    }
    
    fColor = vec4(0.f);
    vec4 diff_color = texture(u_diffuseMap, vTexCoord);
    vec3 vertex_camera = (texture(u_vertexMap, vTexCoord)).xyz;
    
    /*for(int i = 0; i < u_numLights; i++) {
      vec3 light_camera = (viewMatrix * u_lightPos[i]).xyz;
      vec3 light_dir = light_camera - vertex_camera;
      float dist = length(light_dir);
      light_dir = normalize(light_dir);
      
      float diff = max(dot(normal_camera, light_dir), 0.f);
      fColor.rgb += diff * diff_color.rgb / (dist);
      
      if(diff > 0.f) {
        vec3 V = normalize(-vertex_camera);
        vec3 R = normalize(reflect(-light_dir, normal_camera));
        
        float angle = max(dot(R, V), 0.f);
        float spec = pow(angle, 128.f);
        vec3 specColor = vec3(spec);
        fColor.rgb += specColor;
      }
    }*/
    vec3 ambientColor = vec3(1 - occ_factor);
    fColor.rgb += ambientColor;
  }
 }
