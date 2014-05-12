#include "harris.h"
#include "logger.h"

bool ApplyKernel(Image* src_img, Image* dst_img, float* kernel, size_t kernel_size);
bool Sobel(Image* src_img, Image* dst_img);

bool HarrisCornerDetector(Image* src_img, Image* dst_img)
{
  if(src_img == NULL || dst_img == NULL) {
    Logger::getInstance()->error("HarrisCornerDetector -> one of the images is NULL. Aborting function.");
    return false;
  }

  Sobel(src_img, dst_img);

  /*int w = imgGetWidth(src_img);
  int h = imgGetHeight(src_img);*/

  return true;
}

bool ApplyKernel(Image* src_img, Image* dst_img, float* kernel, size_t kernel_size)
{
  Logger::getInstance()->warn("ApplyKernel not fully implemented yet. Returning false.");

  int w = imgGetWidth(src_img);
  int h = imgGetHeight(src_img);
  float* src_data = imgGetData(src_img);
  float* dst_data = imgGetData(dst_img);

  for(int i = 1; i < h-1; i++) {
    for(int j = 1; j < w-1; j++) {
      //dst_data[i * w + j] = src_data[i * w + j];
      float tmp = kernel[0] * src_data[(i-1) * w + (j-1)];
      tmp += kernel[1] * src_data[(i-1) * w + j];
      tmp += kernel[2] * src_data[(i-1) * w + (j+1)];

      tmp += kernel[3] * src_data[i * w + (j-1)];
      tmp += kernel[4] * src_data[i * w + j];
      tmp += kernel[5] * src_data[i * w + (j+1)];

      tmp += kernel[6] * src_data[(i+1) * w + (j-1)];
      tmp += kernel[7] * src_data[(i+1) * w + j];
      tmp += kernel[8] * src_data[(i+1) * w + (j+1)];

      dst_data[i * w + j] = tmp;
    }
  }

  return false;
}

bool Sobel(Image* src_img, Image* dst_img)
{
  if(src_img == NULL || dst_img == NULL) {
    Logger::getInstance()->error("Sobel -> one of the images is NULL. Aborting function.");
    return false;
  }

  float k_dx[9] = {-1, 0, 1,
                   -2, 0, 2,
                   -1, 0, 1};

  float k_dy[9] = { 1,  2,  1,
                    0,  0,  0,
                   -1, -2, -1};

  int w = imgGetWidth(src_img);
  int h = imgGetHeight(src_img);

  Image* dx_img = imgCreate(w, h, 1);
  Image* dy_img = imgCreate(w, h, 1);

  ApplyKernel(src_img, dx_img, k_dx, 9);
  ApplyKernel(src_img, dy_img, k_dy, 9);

  float* dx_img_data = imgGetData(dx_img);
  float* dy_img_data = imgGetData(dy_img);
  float* dst_img_data = imgGetData(dst_img);

  for(int i = 0; i < w * h; i++) {
    dst_img_data[i] = sqrt(dx_img_data[i] * dx_img_data[i] + dy_img_data[i] * dy_img_data[i]);
  }

  return true;
}