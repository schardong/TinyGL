#ifndef HARRIS_H
#define HARRIS_H

#include <vector>
#include <glm/glm.hpp>

extern "C" {
#include "image.h"
}

/**
 * HarrisCornerDetector.
 * This method implements the Harris corner detector algorithm. The source
 * image must be a grayscale image. The destination image is allocated in the
 * function.
 * References:
 * http://www.cse.psu.edu/~rcollins/CSE486/lecture06.pdf
 * http://www.mathworks.com/help/images/ref/corner.html

 * @src_img: The original grayscale image.
 * @dst_img: The final image where only the corners will be visible.
 * @returns: A vector of corner coordinates in the image space.
 */
std::vector<glm::vec2> HarrisCornerDetector(Image* src_img, Image* dst_img);

#endif // HARRIS_H