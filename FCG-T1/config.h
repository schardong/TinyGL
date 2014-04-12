#ifndef CONFIG_H
#define CONFIG_H

//This enum sets in which colorspace the spectrum of color will be rendered.
typedef enum {
  CIEXYZ,
  CIERGB,
  sRGB,
  CIELab,
  numColorspaces
} eColorspace;

//This enum sets which spectrum of color will be rendered on a colorspace.
typedef enum {
  SPECTRUM,
  CIEXYZ,
  CIERGB,
  sRGB,
  CIELab,
  numSpectra
} eSpectrum;

#endif // CONFIG_H
