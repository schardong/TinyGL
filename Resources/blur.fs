#version 330 core

layout (location = 0) out vec4 fColor;

uniform sampler2D u_ssaoMap;
uniform vec2 u_screenSize;

in vec2 vTexCoord;

vec3 GaussBlur(vec2 tex_coord)
{
  float kernel[9] = float[](1, 2, 1,
                            2, 4, 2,
                            1, 2, 1);
                     
  vec3 blurred_color = vec3(0);
  
  vec2 s = 1 / u_screenSize;
  vec2 off[9];
  
  off[0] = -s; off[1] = vec2(0.0, -s.y); off[2] = vec2(s.x, -s.y);
  off[3] = vec2(-s.x, 0.0); off[4] = vec2(0); off[5] = vec2(s.x, 0.0);
  off[6] = vec2(-s.x, s.y); off[7] = vec2(0.0, s.y); off[8] = s;
    
  for(int i = 0; i < 9; i++) {
    blurred_color += (texture(u_ssaoMap, vTexCoord + off[i]).xyz * kernel[i]) / 16;
  }
  
  return blurred_color.rrr;
}

void main()
{
  fColor.rgb = GaussBlur(vTexCoord);
  //fColor.rgb = texture(u_ssaoMap, vTexCoord).rrr;
}