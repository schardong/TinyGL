#include "light.h"

Light::Light()
{
  m_position = m_color = glm::vec3(0);
}

Light::Light(glm::vec3 pos) :
  m_position(pos)
{
  m_color = glm::vec3(0);
}

Light::Light(glm::vec3 pos, glm::vec3 color) :
  m_position(pos),
  m_color(color)
{}

Light::~Light()
{
  m_position = m_color = glm::vec3(0);
}
