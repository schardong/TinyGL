#ifndef CIEXYZMESH_H
#define CIEXYZMESH_H

#include "mesh.h"

class CIExyzMesh : public Mesh
{
public:
  CIExyzMesh(std::vector<glm::vec3> xyz);
  virtual ~CIExyzMesh();
};

#endif // CIEXYZMESH_H
