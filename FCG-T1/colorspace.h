#ifndef COLORSPACE_H
#define COLORSPACE_H

#include <vector>
#include <glm/glm.hpp>
#include "logger.h"

extern "C" {
#include "color.h"
#include "math.h"
}

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

glm::vec3 rgbToCIERGB(glm::vec3 rgb)
{
  glm::vec3 tmp;

  if (rgb.r <= 0.0031308)
    tmp.r = 12.92f * rgb.r;
  else
    tmp.r = 1.055f * pow(rgb.r, 1.f / 2.4f) - 0.055f;

  if (rgb.g <= 0.0031308)
    tmp.g = 12.92f * rgb.g;
  else
    tmp.g = 1.055f * pow(rgb.g, 1.f / 2.4f) - 0.055f;

  if (rgb.b <= 0.0031308)
    tmp.b = 12.92f * rgb.b;
  else
    tmp.b = 1.055f * pow(rgb.b, 1.f / 2.4f) - 0.055f;

  return tmp;
}

glm::vec3 CIEXYZtoCIEsRGB(glm::vec3 ciexyz, glm::vec3 white)
{
  glm::mat3 m(0.4124564, 0.3575761, 0.1804375, 0.2126729, 0.7151522, 0.0721750, 0.0193339, 0.1191920, 0.9503041);

  glm::vec3 xyz = ciexyz;
  xyz.x /= white.x;
  xyz.y /= white.y;
  xyz.z /= white.z;

  glm::vec3 rgb;
  rgb = m * xyz;

  return rgbToCIERGB(rgb);
}

#endif