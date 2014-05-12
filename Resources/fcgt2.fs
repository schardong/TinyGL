#version 330 core

uniform sampler2D u_image;
uniform vec2 u_screenSize;

layout (location = 0) out vec4 fColor;

in vec2 vTexCoord;

vec4 Sobel(sampler2D img, vec2 img_size)
{
  vec3 final_color = vec3(0);
  mat3 kernel_dx = mat3(-1, -2, -1, 0, 0, 0, 1, 2, 1);
  mat3 kernel_dy = transpose(kernel_dx);
  mat3 edge = mat3(-1, -1, -1, -1, 8, -1, -1, -1, -1);

  float off_x = 1 / img_size.x;
  float off_y = 1 / img_size.y;

  vec2 off_mat[9];
  off_mat[0] = vec2(-off_x, -off_y);
  off_mat[1] = vec2(0, -off_y);
  off_mat[2] = vec2(off_x, -off_y);

  off_mat[3] = vec2(-off_x, 0);
  off_mat[4] = vec2(0, 0);
  off_mat[5] = vec2(off_x, 0);

  off_mat[6] = vec2(-off_x, off_y);
  off_mat[7] = vec2(0, off_y);
  off_mat[8] = vec2(off_x, off_y);

  vec3 img_dx = vec3(0);
  vec3 img_dy = vec3(0);

  for(int i = 0; i < 9; i++) {
    vec3 tmp = texture(img, vTexCoord + off_mat[i]).rgb;
    img_dx += tmp * kernel_dx[i];
    img_dy += tmp * kernel_dy[i];
  }

  final_color = sqrt(img_dx * img_dx + img_dy * img_dy);

  return vec4(final_color, 1);
}

void main()
{
  fColor = texture(u_image, vTexCoord);
  //fColor = Sobel(u_image, u_screenSize);
  //fColor = -(dFdx(texture(u_image, vTexCoord)) + dFdy(texture(u_image, vTexCoord)));
}
