#version 330 core

layout (location = 0) in vec3 in_vPosition;

uniform mat4 modelMatrix;
uniform mat4 projMatrix;

void main()
{ 
  mat4 PM = modelMatrix;
  gl_Position = PM * vec4(in_vPosition, 1.0);
}