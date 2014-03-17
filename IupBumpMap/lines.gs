#version 330

layout (points) in;
layout (line_strip, max_vertices = 24) out;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec4 ex_vColor[];
out vec4 ex_gColor[];

void main()
{
  mat4 MVP = projMatrix * viewMatrix * modelMatrix;

  vec4 C = gl_in[0].gl_Position;
  vec4 I = vec4(1.0, 0.0, 0.0, 0.0);
  vec4 J = vec4(0.0, 1.0, 0.0, 0.0);
  vec4 K = vec4(0.0, 0.0, 1.0, 0.0);

  vec4 cube_vert[8];

  cube_vert[0] = C-I-J-K;
  cube_vert[1] = C+I-J-K;
  cube_vert[2] = C+I+J-K;
  cube_vert[3] = C-I+J-K;

  cube_vert[4] = C-I-J+K;
  cube_vert[5] = C+I-J+K;
  cube_vert[6] = C+I+J+K;
  cube_vert[7] = C-I+J+K;

  int back_lines[5] = {0, 1, 2, 3, 0};
  int front_lines[5] = {4, 5, 6, 7, 4};
  int side_lines[8] = {0, 4, 1, 5, 2, 6, 3, 7};

  for(int i = 0; i < 5; i++) {
    gl_Position = MVP * cube_vert[back_lines[i]];
    ex_gColor[0] = cube_vert[back_lines[i]];
    EmitVertex();
  }
  EndPrimitive();

  for(int i = 0; i < 5; i++) {
    gl_Position = MVP * cube_vert[front_lines[i]];
    ex_gColor[0] = cube_vert[front_lines[i]];
    EmitVertex();
  }
  EndPrimitive();

  for(int i = 0; i < 8; i++) {
    gl_Position = MVP * cube_vert[side_lines[i]];
    ex_gColor[0] = cube_vert[side_lines[i]];
    EmitVertex();
  }
  EndPrimitive();

}