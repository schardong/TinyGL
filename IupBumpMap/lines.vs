#version 330

layout (location = 0) in vec3 in_vPosition;
out vec4 ex_vColor;

void main()
{
  gl_Position = vec4(in_vPosition, 1.0);
  ex_vColor = vec4(in_vPosition, 1.0);
}