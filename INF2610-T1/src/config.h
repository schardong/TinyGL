#ifndef INF2610T1CONFIG_H
#define INF2610T1CONFIG_H

static const int W_SPHERES = 10;
static const int H_SPHERES = 10;
static const int NUM_SPHERES = W_SPHERES * H_SPHERES;
static const int WINDOW_W = 800;
static const int WINDOW_H = 600;

#include <string>

#ifdef WIN32
  static const std::string RESOURCE_PATH = "../Resources";
#else
  static const char* RESOURCE_PATH = "../../Resources";
#endif // WIN32

#endif // INF2610T1CONFIG_H