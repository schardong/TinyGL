#ifndef HARRIS_H
#define HARRIS_H

#include <vector>
#include <glm/glm.hpp>

#include "image.h"

typedef enum {
  HARRIS,
  SHI_TOMASI
} Detector;

/**
 * HarrisCornerDetector.
 * This method implements the Harris and Shi-Tomasi corner detector algorithms.
 * The source image must be a grayscale image. The destination image is
 * allocated in the function.
 * References:
 * http://www.cse.psu.edu/~rcollins/CSE486/lecture06.pdf
 * http://www.mathworks.com/help/images/ref/corner.html

 * @src_img: The original grayscale image.
 * @dst_img: The final image where only the corners will be visible.
 * @d: The corner detector to use. The available detectors are: HARRIS and SHI_TOMASI.
 * @thresh: The threshold to use after the corner response calculation.
 * @returns: A vector of corner coordinates in the image space.
 */
std::vector<glm::vec2> HarrisCornerDetector(Image* src_img, Image* dst_img, Detector d, double thresh);

#endif // HARRIS_H
