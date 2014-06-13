#ifndef AXIS_H
#define AXIS_H

#include <glm/glm.hpp>
#include "mesh.h"

class Axis : public Mesh
{
public:
  Axis(glm::vec2 xlimits, glm::vec2 ylimits, glm::vec2 zlimits);
  virtual ~Axis();
};

#endif // AXIS_H
