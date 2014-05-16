#include "harris.h"
#include "logger.h"
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

bool ApplyKernel(Image* src_img, Image* dst_img, float* kernel, size_t order);
bool Threshold(Image* src_img, Image* dst_img, float t);
float trace(float* m, size_t order);

enum
{
  DX2,
  DY2,
  DXY,
  DXGAUSS,
  DYGAUSS,
  DXYGAUSS,
  R,
  num_images
};

bool HarrisCornerDetector(Image* src_img, Image* dst_img)
{
  if(src_img == NULL || dst_img == NULL) {
    Logger::getInstance()->error("HarrisCornerDetector -> one of the images is NULL. Aborting function.");
    return false;
  }

  float k_dx[9] = {-1, 0, 1,
                   -2, 0, 2,
                   -1, 0, 1};

  float k_dy[9] = { 1,  2,  1,
                    0,  0,  0,
                   -1, -2, -1};

  float k_gauss[25] = {1,  4,  7,  4, 1,
                       4, 16, 26, 16, 4,
                       7, 26, 41, 26, 7,
                       4, 16, 26, 16, 4,
                       1,  4,  7,  4, 1};
  
  for(int i = 0; i < 25; i++) 
    k_gauss[i] /= (float) 273;

  int w = imgGetWidth(src_img);
  int h = imgGetHeight(src_img);
  int img_size = w * h;

  Image** int_img = new Image*[num_images];
  for(int i = DX2; i < num_images; i++) {
    int_img[i] = imgCreate(w, h, 1);
  }

  //Computing the image derivates.
  ApplyKernel(src_img, int_img[DX2], k_dx, 3);
  ApplyKernel(src_img, int_img[DY2], k_dy, 3);

  float* dx_img_data = imgGetData(int_img[DX2]);
  float* dy_img_data = imgGetData(int_img[DY2]);
  float* dxy_img_data = imgGetData(int_img[DXY]);

  for(int i = 0; i < img_size; i++) {
    dx_img_data[i] *= dx_img_data[i];
    dy_img_data[i] *= dy_img_data[i];
    dxy_img_data[i] = dx_img_data[i] * dy_img_data[i];
  }
  
  ApplyKernel(int_img[DX2], int_img[DXGAUSS], k_gauss, 5);
  ApplyKernel(int_img[DY2], int_img[DYGAUSS], k_gauss, 5);
  ApplyKernel(int_img[DXY], int_img[DXYGAUSS], k_gauss, 5);

  dx_img_data = imgGetData(int_img[DXGAUSS]);
  dy_img_data = imgGetData(int_img[DYGAUSS]);
  dxy_img_data = imgGetData(int_img[DXYGAUSS]);

  glm::mat2* harris_mat = new glm::mat2[img_size];

  for(int i = 0; i < img_size; i++) {
    harris_mat[i][0][0] = dx_img_data[i];
    harris_mat[i][0][1] = dxy_img_data[i];
    harris_mat[i][1][0] = dxy_img_data[i];
    harris_mat[i][1][1] = dy_img_data[i];
  }

  float* dst_data = imgGetData(dst_img);

  float* r = imgGetData(int_img[R]);
  for(int i = 0; i < img_size; i++) {
    float t = (float) trace(glm::value_ptr(harris_mat[i]), 2);
    float d = glm::determinant(harris_mat[i]);

    r[i] = d - (0.004f * t * t);
  }

  Threshold(int_img[R], dst_img, 0.9f);
  
  for(int i = DX2; i < num_images; i++) {
    imgDestroy(int_img[i]);
    int_img[i] = NULL;
  }
  delete[] int_img;
  delete[] harris_mat;
  return true;
}

bool ApplyKernel(Image* src_img, Image* dst_img, float* kernel, size_t order)
{
  float* src_data = imgGetData(src_img);
  float* dst_data = imgGetData(dst_img);
  int w = imgGetWidth(src_img);
  int h = imgGetHeight(src_img);

  int limit = (int)ceil(order / 2);

  for(int i = limit; i < h - limit; i++) {
    for(int j = limit; j < w - limit; j++) {
      float tmp = 0;

      for(int k = -limit; k <= limit; k++)
        for(int l = -limit; l <= limit; l++)
          tmp += kernel[(k + limit) * order + (l + limit)] * src_data[(i + k) * w + (j + l)];

      dst_data[i * w + j] = tmp;
    }
  }

  return false;
}

bool Threshold(Image* src_img, Image* dst_img, float t)
{
  if(src_img == NULL || dst_img == NULL || t <= 0.f || t >= 1.f) {
    Logger::getInstance()->error("Threshold -> invalid values passed. Aborting function.");
    return false;
  }

  int img_size = imgGetWidth(src_img) * imgGetHeight(src_img);
  float* src_data = imgGetData(src_img);
  float* dst_data = imgGetData(dst_img);

  for(int i = 0; i < img_size; i++) {
    dst_data[i] = src_data[i] > t ? 1.f : 0.f;
  }

  return true;
}

float trace(float* m, size_t order) {

  size_t s = order * order;
  float t = 0.0;

  for(int i = 0; i < s; i += (order + 1))
    t += m[i];

  return t;
}