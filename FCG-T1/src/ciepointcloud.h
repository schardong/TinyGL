#ifndef CIEPOINTCLOUD_H
#define CIEPOINTCLOUD_H

#include "mesh.h"
#include <glm/glm.hpp>

class CIEPointCloud : public Mesh
{
public:
  CIEPointCloud(std::vector<glm::vec3> points);
  CIEPointCloud(std::vector<glm::vec3> points, std::vector<glm::vec3> colors);
  virtual ~CIEPointCloud();
};

#endif