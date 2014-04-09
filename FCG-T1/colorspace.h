#ifndef COLORSPACE_H
#define COLORSPACE_H

#include <vector>
#include <glm/glm.hpp>
#include "logger.h"

glm::vec3 createCIEXYZ(float* beta, float* illuminant, std::vector<glm::vec3> xyzbar, size_t step)
{
  if (!beta || !illuminant || xyzbar.empty() || !step) {
    Logger::getInstance()->warn("createCIEXYZ() -> One of the parameters is invalid, nothing will be done.");
    return glm::vec3(0, 0, 0);
  }

  glm::vec3 ciexyz(0, 0, 0);
  float n = 0.f;
  for (size_t i = 0; i < 400; i += step) {
    float p = beta[i] * illuminant[i];
    ciexyz.x += p * xyzbar[i].x;
    ciexyz.y += p * xyzbar[i].y;
    ciexyz.z += p * xyzbar[i].z;
    n += illuminant[i] * xyzbar[i].y;
  }

  ciexyz.x /= n;
  ciexyz.y /= n;
  ciexyz.z /= n;

  return ciexyz;
}

#endif