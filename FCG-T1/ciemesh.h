#ifndef CIEXYZMESH_H
#define CIEXYZMESH_H

#include "mesh.h"

extern "C" {
#include "color.h"
}

class CIEMesh : public Mesh
{
public:
  CIEMesh(std::vector<glm::vec3> xyz, size_t slices);
  CIEMesh(std::vector<glm::vec3> xyz, std::vector<glm::vec3> rgb, size_t slices);
  virtual ~CIEMesh();
};

#endif // CIEXYZMESH_H
