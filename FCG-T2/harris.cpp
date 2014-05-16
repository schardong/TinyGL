#include "harris.h"
#include "logger.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define _USE_MATH_DEFINES
extern "C" {
#include <math.h>
}


enum
{
  DX,
  DY,
  DXY,
  DXGAUSS,
  DYGAUSS,
  DXYGAUSS,
  R,
  THRESH,
  num_images
};

bool ApplyKernel(Image* src_img, Image* dst_img, float* kernel, size_t order);
bool Threshold(Image* src_img, Image* dst_img, float t);
bool NonmaximaSuppression(Image* src_img, Image* dst_img);
float trace(float* m, size_t order);

std::vector<glm::vec2> HarrisCornerDetector(Image* src_img, Image* dst_img)
{
  std::vector<glm::vec2> corners;
  if(src_img == NULL || dst_img == NULL) {
    Logger::getInstance()->error("HarrisCornerDetector -> one of the images is NULL. Aborting function.");
    return corners;
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
  for(int i = DX; i < num_images; i++) {
    int_img[i] = imgCreate(w, h, 1);
  }

  //Computing the image derivates.
  ApplyKernel(src_img, int_img[DX], k_dx, 3);
  ApplyKernel(src_img, int_img[DY], k_dy, 3);

  float* dx_img_data = imgGetData(int_img[DX]);
  float* dy_img_data = imgGetData(int_img[DY]);
  float* dxy_img_data = imgGetData(int_img[DXY]);

  for(int i = 0; i < img_size; i++) {
    dx_img_data[i] *= dx_img_data[i];
    dy_img_data[i] *= dy_img_data[i];
    dxy_img_data[i] = dx_img_data[i] * dy_img_data[i];
  }
  
  ApplyKernel(int_img[DX], int_img[DXGAUSS], k_gauss, 5);
  ApplyKernel(int_img[DY], int_img[DYGAUSS], k_gauss, 5);
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

  Threshold(int_img[R], int_img[THRESH], 0.9f);
  NonmaximaSuppression(int_img[THRESH], dst_img);

  for(int i = 0; i < h; i++)
    for(int j = 0; j < w; j++)
      if(dst_data[i * w + j] >= 0.9f) corners.push_back(glm::vec2(i, j));
  
  for(int i = DX; i < num_images; i++) {
    imgDestroy(int_img[i]);
    int_img[i] = NULL;
  }
  delete[] int_img;
  delete[] harris_mat;
  return corners;
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

bool NonmaximaSuppression(Image* src_img, Image* dst_img)
{
  if(src_img == NULL || dst_img == NULL) {
    Logger::getInstance()->error("NonmaximaSuppresion -> invalid values passed. Aborting function.");
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
  int img_size = w * h;
  
  Image** img = new Image*[2];
  img[DX] = imgCreate(w, h, 1);
  img[DY] = imgCreate(w, h, 1);

  float* src_data = imgGetData(src_img);
  float* dx_data = imgGetData(img[DX]);
  float* dy_data = imgGetData(img[DY]);
  float* dst_data = imgGetData(dst_img);

  ApplyKernel(src_img, img[DX], k_dx, 3);
  ApplyKernel(src_img, img[DY], k_dy, 3);

  float* theta = new float[img_size];
  memset(theta, 0.f, sizeof(float) * img_size);
  for(int i = 0; i < img_size; i++) {
    float angle = 0;
    if(dx_data[i] != 0) angle = atan(dy_data[i] / dx_data[i]);



    /*float intpart;
    float floatpart = modf(angle, &intpart);
    dst_data[i] = floatpart;
    if(intpart != 0.f || floatpart != 0.f) {
      std::cout << M_PI / (angle) << "   ";
    }*/
    //std::cout << dy_data[i] / dx_data[i] << " ";
    
  }

  /*for(int i = order; i < h - order; i++) {
    for(int j = order; j < w - order; j++) {

    }
  }*/

  imgDestroy(img[DX]);
  imgDestroy(img[DY]);
  img[DX] = img[DY] = NULL;
  delete[] img;
  return true;
}

float trace(float* m, size_t order) {

  size_t s = order * order;
  float t = 0.0;

  for(size_t i = 0; i < s; i += (order + 1))
    t += m[i];

  return t;
}