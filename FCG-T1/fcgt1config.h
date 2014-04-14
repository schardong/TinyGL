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

//This enum sets which spectrum of color will be rendered in a given colorspace.
namespace spectrum
{
  enum
  {
    SPECTRUM,
    CIEXYZ,
    CIERGB,
    sRGB,
    CIELab,
    n_spectra
  };
};

#endif