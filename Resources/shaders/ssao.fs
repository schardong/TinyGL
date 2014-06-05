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

#define M_PI 3.1415926535897932384626433832795

const int g_maxLights = 50;

in vec2 vTexCoord;

layout (std140) uniform LightPos
{
  vec4 u_lightPos[g_maxLights];
};

const int g_sampleCount = 16;
const int g_radius = 16;

const vec2 g_poissonDisk[] = vec2[](
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

const vec3 g_poissonSphere[] = vec3[](
	vec3(0.53812504, 0.18565957, -0.43192),
	vec3(0.13790712, 0.24864247, 0.44301823),
	vec3(0.33715037, 0.56794053, -0.005789503),
	vec3(-0.6999805, -0.04511441, -0.0019965635),
	vec3(0.06896307, -0.15983082, -0.85477847),
	vec3(0.056099437, 0.006954967, -0.1843352),
	vec3(-0.014653638, 0.14027752, 0.0762037),
	vec3(0.010019933, -0.1924225, -0.034443386),
	vec3(-0.35775623, -0.5301969, -0.43581226),
	vec3(-0.3169221, 0.106360726, 0.015860917),
	vec3(0.010350345, -0.58698344, 0.0046293875),
	vec3(-0.08972908, -0.49408212, 0.3287904),
	vec3(0.7119986, -0.0154690035, -0.09183723),
	vec3(-0.053382345, 0.059675813, -0.5411899),
	vec3(0.035267662, -0.063188605, 0.54602677),
	vec3(-0.47761092, 0.2847911, -0.0271716)
);

float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float linearizeDepth(vec2 texcoord)
{
  float z = 2.0 * u_zNear/ (u_zFar + u_zNear - texture2D(u_depthMap, texcoord).x * (u_zFar - u_zNear));
  return z;
}

mat4 rotationMatrix(float angle, vec3 axis)
{
  axis = normalize(axis);
  float s = sin(angle);
  float c = cos(angle);
  float oc = 1.0 - c;
    
  return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
              oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
              oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
              0.0,                                0.0,                                0.0,                                1.0);
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
    
    float angle = rand(vTexCoord) * 2 * M_PI;
    
    mat2 rot_mat = mat2(rotationMatrix(angle, vec3(0, 0, 1)));
    
    for(int i = 0; i < g_sampleCount; i++) {
      float depth = texture(u_depthMap, vTexCoord).r;
      vec2 sampleTexCoord = vTexCoord + ((rot_mat * g_poissonDisk[i]) * g_radius / u_screenSize.x);
      vec3 samplePos = texture(u_vertexMap, sampleTexCoord).xyz;
      vec3 sampleNormal = texture(u_normalMap, sampleTexCoord).xyz;
      
      occ_factor += calcOcclusion(vertex_camera, normal_camera, samplePos, sampleNormal);
    }
    
    fColor = 1 - 2 * occ_factor / g_sampleCount;
  }
 }
