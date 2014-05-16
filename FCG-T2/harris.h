#ifndef HARRIS_H
#define HARRIS_H

#include <vector>
#include <glm/glm.hpp>

extern "C" {
#include "image.h"
}

std::vector<glm::vec2> HarrisCornerDetector(Image* src_img, Image* dst_img);

#endif // HARRIS_H