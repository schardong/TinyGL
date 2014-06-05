#version 330 core

layout (location = 0) out vec4 fColor;

uniform sampler2D u_ssaoMap;
uniform vec2 u_screenSize;

in vec2 vTexCoord;

float GaussianCoeff(int i)
{
  if(i < 0 || i >= 9) return 0;
  float kernel[9] = float[](1, 2, 1, 2, 4, 2, 1, 2, 1);
  return kernel[i] / 16;
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
  for(int i = 0; i < 9; i++) {
    vec3 curr_sample = texture(u_ssaoMap, tex_coord + ComputeOffset(i)).xyz;
    float gauss_coeff = GaussianCoeff(i);
    
    float closeness = distance(curr_sample, vec3(blurred_color)) / length(vec3(1));
    
    float sample_w = closeness * gauss_coeff;
    blurred_color += curr_sample * sample_w;
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