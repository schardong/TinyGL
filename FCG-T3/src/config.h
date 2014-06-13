#ifndef FCGT3CONFIG_H
#define FCGT3CONFIG_H

#include <string>

#ifdef WIN32
  static const std::string RESOURCE_PATH = "../Resources";
#else
  static const char* RESOURCE_PATH = "../../Resources";
#endif // WIN32

#define DEG2RAD(x) (M_PI * x / 180)

const std::string WINDOW_TITLE = "FCG-T3";
static int WINDOW_W = 640;
static int WINDOW_H = 480;
const int NUM_PATTERNS = 24;
const int NUM_IMAGES = NUM_PATTERNS;

#endif
