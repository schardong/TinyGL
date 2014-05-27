#version 330 core

uniform sampler2D u_image;
uniform vec2 u_screenSize;

layout (location = 0) out vec4 fColor;

in vec2 vTexCoord;

void main()
{
  fColor = texture(u_image, vTexCoord).rrra;
}
