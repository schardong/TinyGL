#version 330 core

layout (location = 0) in vec3 in_vPosition;

uniform mat4 MVP;
uniform vec4 u_materialColor;

out vec4 vColor;

void main()
{
  vColor = u_materialColor;
  gl_Position = MVP * vec4(in_vPosition, 1.f);
}
