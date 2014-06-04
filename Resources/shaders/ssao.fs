#version 330 core

layout (location = 0) out float  fColor;

uniform sampler2D u_diffuseMap;
uniform sampler2D u_normalMap;
uniform sampler2D u_vertexMap;
uniform sampler2D u_depthMap;
uniform sampler2D u_rndNormalMap;

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

const int g_sampleCount = 16;
const int g_radius = 8;

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
                               
float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float linearizeDepth(vec2 texcoord)
{
  float z = 2.0 * u_zNear/ (u_zFar + u_zNear - texture2D(u_depthMap, texcoord).x * (u_zFar - u_zNear));
  return z;
}


/**
 * calcOcclusion - Calculates the occlusion factor given the point's position
 * and normal vectors and the occluder's position and normal vectors.
 */
float calcOcclusion(vec3 pt_pos, vec3 pt_normal, vec3 occ_pos, vec3 occ_normal)
{
  vec3 occ_dir = occ_pos - pt_pos;
  float d = length(occ_dir);
  occ_dir = normalize(occ_dir);
  
  return max(0.0, dot(pt_normal, occ_dir)) / (1.0 + d);
}
                               
void main()
{
  vec3 normal_camera = (texture(u_normalMap, vTexCoord)).xyz;
  
  if(length(normal_camera) == 0)
    fColor = (0.8);
  else {
    float occ_factor = 0;
    vec3 vertex_camera = (texture(u_vertexMap, vTexCoord)).xyz;
        
    for(int i = 0; i < g_sampleCount; i++) {
      //float depth = texture(u_depthMap, vTexCoord).r;
      vec2 sampleTexCoord = vTexCoord + (g_poissonSamples[i] * g_radius / u_screenSize.x);
      vec3 samplePos = texture(u_vertexMap, sampleTexCoord).xyz;
      vec3 sampleNormal = texture(u_normalMap, sampleTexCoord).xyz;
      
      /*vec3 V = samplePos - vertex_camera;
      float d = length(V);
      V = normalize(V);*/
      
      occ_factor += calcOcclusion(vertex_camera, normal_camera, samplePos, sampleNormal);//max(0.0, dot(normal_camera, V)) / (1.0 + d);
    }
    
    fColor = 1 - 2*occ_factor / g_sampleCount;
  }
 }
