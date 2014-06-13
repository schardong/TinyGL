#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

class Light
{
public:
  Light();
  Light(glm::vec3 pos);
  Light(glm::vec3 pos, glm::vec3 color);

  ~Light();

  void setPosition(glm::vec3 rhs)
  {
    m_position = rhs;
  }

  void setColor(glm::vec3 rhs)
  {
    m_color = rhs;
  }

  glm::vec3 getPosition()
  {
    return m_position;
  }

  glm::vec3 getColor()
  {
    return m_color;
  }

private:
  glm::vec3 m_position;
  glm::vec3 m_color;
};

#endif // LIGHT_H