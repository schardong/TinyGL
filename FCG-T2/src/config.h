#ifndef FCGT2CONFIG_H
#define FCGT2CONFIG_H

#include <string>

#ifdef WIN32
  static const std::string RESOURCE_PATH = "../Resources";
#else
  static const char* RESOURCE_PATH = "../../Resources";
#endif // WIN32

const std::string WINDOW_TITLE = "FCG-T2";
static int WINDOW_W = 800;
static int WINDOW_H = 600;
const int NUM_PATTERNS = 9;
const int NUM_FID = 4;
const int NUM_SOCCER = 1;
const int NUM_IMAGES = NUM_PATTERNS + NUM_FID + NUM_SOCCER;

#endif