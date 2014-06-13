#ifndef INF2610T4CONFIG_H
#define INF2610T4CONFIG_H

static const int W_SPHERES = 10;
static const int H_SPHERES = 10;
static const int NUM_SPHERES = W_SPHERES * H_SPHERES;
static const int NUM_LIGHTS = 2;
static int WINDOW_W = 700;
static int WINDOW_H = 700;

#include <string>

#ifdef WIN32
  static const std::string RESOURCE_PATH = "../Resources";
#else
  static const char* RESOURCE_PATH = "../../Resources";
#endif // WIN32

#endif // INF2610T4CONFIG_H