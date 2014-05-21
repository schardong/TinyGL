#version 330 core

layout (location = 0) out vec4 fColor;

uniform sampler2D u_ssaoMap;
uniform vec2 u_screenSize;

in vec2 vTexCoord;

vec3 GaussBlur(vec2 tex_coord)
{
  float kernel[9] = {1, 2, 1,
                     2, 4, 2,
                     1, 2, 1};
                     
  vec3 blurred_color = vec3(0);
  
  float mStepW = 1.0 / u_screenSize.x;
  float mStepH = 1.0 / u_screenSize.y;
  vec2 mOffset[9];
  
  mOffset[0] = vec2 (-mStepW, -mStepH); mOffset[1] = vec2 (0.0, -mStepH); mOffset[2] = vec2 (mStepW, -mStepH);
  mOffset[3] = vec2 (-mStepW, 0.0);     mOffset[4] = vec2 (0.0, 0.0);     mOffset[5] = vec2 (mStepW, 0.0);
  mOffset[6] = vec2 (-mStepW, mStepH);  mOffset[7] = vec2 (0.0, mStepH);  mOffset[8] = vec2 (mStepW, mStepH);
    
  for(int i = 0; i < 9; i++) {
    blurred_color += (texture(u_ssaoMap, vTexCoord + mOffset[i]).xyz * kernel[i]) / 16;
  }
  
  return blurred_color;
}

void main()
{
  //fColor = GaussBlur(vTexCoord);
  fColor = vec4(1.f);
}