#ifndef SPHERE_H
#define SPHERE_H

#include "mesh.h"

/**
* Class Sphere, inherits from Mesh
* This class builds a sphere of radius 1 centered at (0,0,0), given the number
* of horizontal and vertical subdivisions. The draw function is given as a
* callback to the Mesh.
*/
class Sphere : public Mesh
{
public:
  Sphere(int slices, int stacks);
  virtual ~Sphere();
};

#endif