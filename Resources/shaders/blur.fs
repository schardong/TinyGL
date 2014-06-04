#version 330 core

layout (location = 0) out vec4 fColor;

uniform sampler2D u_ssaoMap;
uniform vec2 u_screenSize;

in vec2 vTexCoord;

float GaussBlur(vec2 tex_coord)
{
  float kernel[9] = float[](1.f/16, 2.f/16, 1.f/16,
                            2.f/16, 4.f/16, 2.f/16,
                            1.f/16, 2.f/16, 1.f/16);
                     
  float blurred_color = 0;
  
  vec2 s = 1.f / u_screenSize;
  vec2 off[9];
  
  off[0] = vec2(-s.x, -s.y); off[1] = vec2(0.0, -s.y); off[2] = vec2(s.x, -s.y);
  off[3] = vec2(-s.x, 0.0);  off[4] = vec2(0);         off[5] = vec2(s.x, 0.0);
  off[6] = vec2(-s.x, s.y);  off[7] = vec2(0.0, s.y);  off[8] = vec2(s.x, s.y);
    
  for(int i = 0; i < 9; i++) {
    blurred_color += (texture(u_ssaoMap, vTexCoord + off[i]).x * kernel[i]) / 1;
  }
  
  return blurred_color;
}

void main()
{
  fColor = vec4(vec3(GaussBlur(vTexCoord)), 1.f);
  //fColor.rgb = texture(u_ssaoMap, vTexCoord).rrr;
}