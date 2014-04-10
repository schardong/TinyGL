#ifndef CIEXYZMESH_H
#define CIEXYZMESH_H

#include "mesh.h"

class CIEMesh : public Mesh
{
public:
  CIEMesh(std::vector<glm::vec3> xyz, size_t slices);
  virtual ~CIEMesh();
};

#endif // CIEXYZMESH_H
