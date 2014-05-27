#version 330 core

layout (location = 0) in vec3 in_vPosition;
layout (location = 1) in vec2 in_vTexCoord;

out vec2 vTexCoord;

void main()
{
  vTexCoord = in_vTexCoord;
  gl_Position = vec4(in_vPosition, 1.f);
}
