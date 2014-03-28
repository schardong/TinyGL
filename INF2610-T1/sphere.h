#ifndef SPHERE_H
#define SPHERE_H

#include "mesh.h"

class Sphere : public Mesh
{
public:
  Sphere(int nx, int ny);
  virtual ~Sphere();
};

#endif