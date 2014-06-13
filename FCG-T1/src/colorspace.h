#ifndef COLORSPACE_H
#define COLORSPACE_H

#include <vector>
#include <glm/glm.hpp>
#include <cmath>

#include "color.h"
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

glm::vec3 createCIEXYZPureSource(float* illuminant, std::vector<glm::vec3> xyzbar, size_t step)
{
  if (!illuminant || xyzbar.empty() || step == 0) {
    Logger::getInstance()->warn("createCIEXYZPureSource() -> One of the parameters is invalid, nothing will be done.");
    return glm::vec3(0, 0, 0);
  }

  glm::vec3 ciexyz(0, 0, 0);
  float n = 0.f;
  for (size_t i = 0; i < 400; i += step) {
    ciexyz.x += illuminant[i] * xyzbar[i].x;
    ciexyz.y += illuminant[i] * xyzbar[i].y;
    ciexyz.z += illuminant[i] * xyzbar[i].z;
    n += illuminant[i] * xyzbar[i].y;
  }

  if (n != 0) {
    ciexyz.x /= n;
    ciexyz.y /= n;
    ciexyz.z /= n;
  }
  else
    ciexyz = glm::vec3(0);

  return ciexyz;
}

glm::vec3 CIERGBtoCIEXYZ(glm::vec3 XYZ)
{
  glm::mat3 m(0.490, 0.310, 0.200,  0.177, 0.813, 0.011, 0.000, 0.010, 0.990);
  glm::vec3 RGB = m * XYZ;
  return RGB;
}

glm::vec3 CIEXYZtoCIERGB(glm::vec3 RGB)
{
  glm::mat3 m = glm::inverse(glm::mat3(0.490, 0.310, 0.200,  0.177, 0.813, 0.011, 0.000, 0.010, 0.990));
  glm::vec3 XYZ = m * RGB;
  return XYZ;
}

#endif
