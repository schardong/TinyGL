#include "harris.h"
#include "logger.h"

bool ApplyKernel(Image* src_img, Image* dst_img, float* kernel, size_t kernel_size);

bool HarrisCornerDetector(Image* src_img, Image* dst_img)
{
  if(src_img == NULL || dst_img == NULL) {
    Logger::getInstance()->error("HarrisCornerDetector -> onde of the images is NULL. Aborting function.");
    return false;
  }

  float k_dx[9] = {-1, 0, 1,
                   -1, 0, 1,
                   -1, 0, 1};

  float k_dy[9] = {-1, -1, -1,
                    0,  0,  0,
                    1,  1,  1};

  int w = imgGetWidth(src_img);
  int h = imgGetHeight(src_img);

  Image* dx_img = imgCreate(w, h, 1);
  Image* dy_img = imgCreate(w, h, 1);
  ApplyKernel(src_img, dx_img, k_dx, 9);
  ApplyKernel(src_img, dy_img, k_dy, 9);



  return true;
}

bool ApplyKernel(Image* src_img, Image* dst_img, float* kernel, size_t kernel_size)
{
  Logger::getInstance()->warn("ApplyKernel not fully implemented yet. Returning false.");

  int w = imgGetWidth(src_img);
  int h = imgGetHeight(src_img);
  float* src_data = imgGetData(src_img);
  float* dst_data = imgGetData(dst_img);

  for(int i = 1; i < h; i++) {
    for(int j = 1; j < w; j++) {
      dst_data[i * h + j] = src_data[i * h + j];
    }
  }

  return false;
}