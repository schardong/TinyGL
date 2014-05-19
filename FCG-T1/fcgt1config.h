#ifndef FCGT1CONFIG_H
#define FCGT1CONFIG_H

//This enum sets in which colorspace the spectrum of color will be rendered.
namespace colorspace
{
  enum
  {
    CIEXYZ,
    CIERGB,
    sRGB,
    CIELab,
    n_colorspaces
  };
};

#endif