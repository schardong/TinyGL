#version 330 core

layout (location = 0) out vec4 fColor;

uniform sampler2D u_ssaoMap;
uniform vec2 u_screenSize;

in vec2 vTexCoord;

float GaussianCoeff7(int i)
{
  if(i < 0 || i >= 49) return 0;
  float kernel[49] = float[](0.001, 0.004, 0.008, 0.010, 0.008, 0.004, 0.001,
  0.004, 0.012, 0.024, 0.030, 0.024, 0.012, 0.004,
  0.008, 0.024, 0.047, 0.059, 0.047, 0.024, 0.008,
  0.010, 0.030, 0.059, 0.073, 0.059, 0.030, 0.010,
  0.008, 0.024, 0.047, 0.059, 0.047, 0.024, 0.008,
  0.004, 0.012, 0.024, 0.030, 0.024, 0.012, 0.004,
  0.001, 0.004, 0.008, 0.010, 0.008, 0.004, 0.001);
  
  return kernel[i];
}

float GaussianCoeff(int i)
{
  if(i < 0 || i >= 9) return 0;
  float kernel[9] = float[](1, 2, 1, 2, 4, 2, 1, 2, 1);
  return kernel[i] / 16;
}

vec2 ComputeOffset7(int i)
{
  if(i < 0 || i >= 49) return vec2(0, 0);
  
  vec2 s = 1.f / u_screenSize;
  vec2 off[49];
  off[0] = vec2(-3 * s.x, -3 * s.y); off[1] = vec2(-2 * s.x, -3 * s.y); off[2] = vec2(-s.x, -3 * s.y), off[3] = vec2(0, -3 * s.y); off[4] = vec2(s.x, -3 * s.y); off[5] = vec2(2 * s.x, -3 * s.y); off[6] = vec2(3 * s.x, -3 * s.y);
  off[7] = vec2(-3 * s.x, -2 * s.y); off[8] = vec2(-2 * s.x, -2 * s.y); off[9] = vec2(-s.x, -2 * s.y), off[10] = vec2(0, -2 * s.y); off[11] = vec2(s.x, -2 * s.y); off[12] = vec2(2 * s.x, -2 * s.y); off[13] = vec2(3 * s.x, -2 * s.y);
  off[14] = vec2(-3 * s.x, -1 * s.y); off[15] = vec2(-2 * s.x, -1 * s.y); off[16] = vec2(-s.x, -1 * s.y), off[17] = vec2(0, -1 * s.y); off[18] = vec2(s.x, -1 * s.y); off[19] = vec2(2 * s.x, -1 * s.y); off[20] = vec2(3 * s.x, -1 * s.y);
  off[21] = vec2(-3 * s.x, 0 * s.y); off[22] = vec2(-2 * s.x, 0 * s.y); off[23] = vec2(-s.x, 0 * s.y), off[24] = vec2(0, 0 * s.y); off[25] = vec2(s.x, 0 * s.y); off[26] = vec2(2 * s.x, 0 * s.y); off[27] = vec2(3 * s.x, 0 * s.y);
  off[28] = vec2(-3 * s.x, 1 * s.y); off[29] = vec2(-2 * s.x, 1 * s.y); off[30] = vec2(-s.x, 1 * s.y), off[31] = vec2(0, 1 * s.y); off[32] = vec2(s.x, 1 * s.y); off[33] = vec2(2 * s.x, 1 * s.y); off[34] = vec2(3 * s.x, 1 * s.y);
  off[35] = vec2(-3 * s.x, 2 * s.y); off[36] = vec2(-2 * s.x, 2 * s.y); off[37] = vec2(-s.x, 2 * s.y), off[38] = vec2(0, 2 * s.y); off[39] = vec2(s.x, 2 * s.y); off[40] = vec2(2 * s.x, 2 * s.y); off[41] = vec2(3 * s.x, 2 * s.y);
  off[42] = vec2(-3 * s.x, 3 * s.y); off[43] = vec2(-2 * s.x, 3 * s.y); off[44] = vec2(-s.x, 3 * s.y), off[45] = vec2(0, 3 * s.y); off[46] = vec2(s.x, 3 * s.y); off[47] = vec2(2 * s.x, 3 * s.y); off[48] = vec2(3 * s.x, 3 * s.y);
  
  return off[i];
}

vec2 ComputeOffset(int i)
{
  if(i < 0 || i >= 9) return vec2(0, 0);
  
  vec2 s = 1.f / u_screenSize;
  vec2 off[9];
  off[0] = vec2(-s.x, -s.y); off[1] = vec2(0.0, -s.y); off[2] = vec2(s.x, -s.y);
  off[3] = vec2(-s.x, 0.0);  off[4] = vec2(0);         off[5] = vec2(s.x, 0.0);
  off[6] = vec2(-s.x, s.y);  off[7] = vec2(0.0, s.y);  off[8] = vec2(s.x, s.y);
  
  return off[i];
}

float GaussBlur(vec2 tex_coord)
{                     
  float blurred_color = 0;
  
  for(int i = 0; i < 9; i++) {
    blurred_color += texture(u_ssaoMap, tex_coord + ComputeOffset(i)).x * GaussianCoeff(i);
  }
  
  return blurred_color;
}

float BilateralBlur(vec2 tex_coord)
{
  float blurred_color = texture(u_ssaoMap, tex_coord).r;
  float norm_fact = 1;
  for(int i = 0; i < 49; i++) {
    vec3 curr_sample = texture(u_ssaoMap, tex_coord + ComputeOffset(i)).xyz;
    float gauss_coeff = GaussianCoeff(i);
    
    float closeness = distance(curr_sample, vec3(blurred_color)) / length(vec3(1));
    
    float sample_w = closeness * gauss_coeff;
    blurred_color += curr_sample.x * sample_w;
    norm_fact += sample_w;
  }
  
  return blurred_color / norm_fact;
}

void main()
{
  //fColor = vec4(vec3(GaussBlur(vTexCoord)), 1.f);
  fColor = vec4(vec3(BilateralBlur(vTexCoord)), 1.f);
  //fColor.rgb = texture(u_ssaoMap, vTexCoord).rrr;
}