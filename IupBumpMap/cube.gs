#version 330

layout (points) in;
layout (triangle_strip, max_vertices = 24) out;

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

  /*int cube_faces[24] = {
    0, 1, 3, 2,
    3, 2, 7, 6,
    4, 5, 7, 6,
    0, 1, 4, 5,
    0, 4, 3, 7,
    5, 1, 6, 2,
  };*/

  ivec4 cube_faces[6];
  cube_faces[0] = ivec4(0, 1, 3, 2);
  cube_faces[1] = ivec4(3, 2, 7, 6);
  cube_faces[2] = ivec4(4, 5, 7, 6);
  cube_faces[3] = ivec4(0, 1, 4, 5);
  cube_faces[4] = ivec4(0, 4, 3, 7);
  cube_faces[5] = ivec4(5, 1, 6, 2);
    
  /*for(int i = 0; i < 24; i++) {
    gl_Position = MVP * cube_vert[cube_faces[i]];
    ex_gColor[0] = cube_vert[cube_faces[i]];
    EmitVertex();
  }*/

  for(int i = 0; i < 6; i++) {
    for(int j = 0; j < 4; j++) {
      gl_Position = MVP * cube_vert[cube_faces[i][j]];
      ex_gColor[0] = ex_vColor[0];
      EmitVertex();
    }
  }
  
}