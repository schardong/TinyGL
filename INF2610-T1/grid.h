#ifndef GRID_H
#define GRID_H

#include "mesh.h"

class Grid : public Mesh
{
public:
  Grid(int nx, int ny);
  virtual ~Grid();
};

#endif