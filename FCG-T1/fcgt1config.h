#ifndef FCGT1CONFIG_H
#define FCGT1CONFIG_H

#include <string>

const std::string WINDOW_TITLE = "FCG-T1";
static int WINDOW_W = 800;
static int WINDOW_H = 600;

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