#ifndef HARRIS_H
#define HARRIS_H

extern "C" {
#include "image.h"
}

bool HarrisCornerDetector(Image* src_img, Image* dst_img);

#endif // HARRIS_H