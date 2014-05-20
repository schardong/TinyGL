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

in vec2 vTexCoord;

layout (std140) uniform LightPos
{
  vec4 u_lightPos[g_maxLights];
};

const int g_sampleCount = 80;
const int g_radius = 20;

const vec2 g_poissonSamples[] = vec2[](
                                vec2( -0.94201624,  -0.39906216 ),
                                vec2(  0.94558609,  -0.76890725 ),
                                vec2( -0.094184101, -0.92938870 ),
                                vec2(  0.34495938,   0.29387760 ),
                                vec2( -0.91588581,   0.45771432 ),
                                vec2( -0.81544232,  -0.87912464 ),
                                vec2( -0.38277543,   0.27676845 ),
                                vec2(  0.97484398,   0.75648379 ),
                                vec2(  0.44323325,  -0.97511554 ),
                                vec2(  0.53742981,  -0.47373420 ),
                                vec2( -0.26496911,  -0.41893023 ),
                                vec2(  0.79197514,   0.19090188 ),
                                vec2( -0.24188840,   0.99706507 ),
                                vec2( -0.81409955,   0.91437590 ),
                                vec2(  0.19984126,   0.78641367 ),
                                vec2(  0.14383161,  -0.14100790 )
                               );
                               
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
    float occ_factor = 0;
    vec3 vertex_camera = (texture(u_vertexMap, vTexCoord)).xyz;
        
    for(int i = 0; i < g_sampleCount; i++) {
          
      vec2 sampleTexCoord = vTexCoord + (g_poissonSamples[i] * g_radius / u_screenSize.x);
      vec3 samplePos = texture(u_vertexMap, sampleTexCoord).xyz;
      
      vec3 V = samplePos - vertex_camera;
      float d = length(V);
      V = normalize(V);
      
      occ_factor += max(0.0, dot(normal_camera, V)) * (1.0 / (1.0 + d));
    }
    
    /*fColor = vec4(0.f);
    vec4 diff_color = texture(u_diffuseMap, vTexCoord);
    for(int i = 0; i < u_numLights; i++) {
      vec3 light_camera = (viewMatrix * u_lightPos[i]).xyz;
      vec3 light_dir = light_camera - vertex_camera;
      float dist = length(light_dir);
      light_dir = normalize(light_dir);
      
      float diff = max(dot(normal_camera, light_dir), 0.f);
      fColor.rgb += diff * diff_color.rgb;
      
      if(diff > 0.f) {
        vec3 V = normalize(-vertex_camera);
        vec3 R = normalize(reflect(-light_dir, normal_camera));
        
        float angle = max(dot(R, V), 0.f);
        float spec = pow(angle, 128.f);
        vec3 specColor = vec3(spec);
        fColor.rgb += specColor;
      }
    }*/
    
    fColor.rgb = vec3(1-(occ_factor / g_sampleCount));
  }
 }