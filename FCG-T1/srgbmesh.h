#ifndef SRGBMESH_H
#define SRGBMESH_H

#include "ciemesh.h"
#include "fcgt1config.h"

class SRGBMesh : public CIEMesh
{
public:
  SRGBMesh();

  void setColorspace(int c)
  {
    if (c < colorspace::CIEXYZ || c >= colorspace::n_colorspaces)
      return;
    m_colorspace = c;
    convertColorspace();
  }

  int getColorspace()
  {
    return m_colorspace;
  }

private:
  int m_colorspace;

  void convertColorspace();
  void buildGeometry();
};

#endif // SRGBMESH_H
