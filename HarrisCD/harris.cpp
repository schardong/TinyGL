#include "harris.h"
#include "logger.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

#define _USE_MATH_DEFINES
extern "C" {
#include <math.h>
}

#define RAD2DEG(x) (180.0f/M_PI * (x))
#define DEG2RAD(x) (M_PI/180.0f * (x))

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
bool NonmaximaSuppression(Image* src_img, Image* dst_img, int neigh_width);
float trace(float* m, size_t order);
vector<float> eigenval(glm::mat2 m);

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
    dxy_img_data[i] = dx_img_data[i] * dy_img_data[i];
    dx_img_data[i] *= dx_img_data[i];
    dy_img_data[i] *= dy_img_data[i];
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
    //Harris detector
    float t = (float) trace(glm::value_ptr(harris_mat[i]), 2);
    float d = glm::determinant(harris_mat[i]);
    r[i] = d - (0.004f * t * t);
    //Shi-tomasi detector
    /*vector<float> e = eigenval(harris_mat[i]);
    r[i] = min(e[0], e[1]);*/
  }

  NonmaximaSuppression(int_img[R], int_img[THRESH], 7);
  Threshold(int_img[THRESH], dst_img, 0.9f);
  
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
    dst_data[i] = src_data[i] > t ? src_data[i] : 0.f;
  }

  return true;
}

bool NonmaximaSuppression(Image* src_img, Image* dst_img, int neigh_width)
{
  if(src_img == NULL || dst_img == NULL || neigh_width <= 0) {
    Logger::getInstance()->error("NonmaximaSuppresion -> invalid values passed. Aborting function.");
    return false;
  }

  int w = imgGetWidth(src_img);
  int h = imgGetHeight(src_img);
  neigh_width = (int)ceil(neigh_width / 2);
  float* src_data = imgGetData(src_img);
  float* dst_data = imgGetData(dst_img);

  for(int i = neigh_width; i < (h - neigh_width); i++) {
    for(int j = neigh_width; j < (w - neigh_width); j++) {
      
      float val = src_data[i * w + j];
      bool max = true;
      
      for(int k = i - neigh_width; k <= (i + neigh_width); k++)
        for(int l = j - neigh_width; l <= (j + neigh_width); l++)
          max = max && (src_data[k * w + l] <= val);
      
      if(!max) continue;

      dst_data[i * w + j] = src_data[i * w + j];
    }
  }

  return true;
}

float trace(float* m, size_t order) {

  size_t s = order * order;
  float t = 0.0;

  for(size_t i = 0; i < s; i += (order + 1))
    t += m[i];

  return t;
}

vector<float> eigenval(glm::mat2 m)
{
  float t = trace(glm::value_ptr(m), 2);
  float d = glm::determinant(m);

  float tmp = sqrt((t*t) / 4 - d);

  vector<float> ev(2);
  ev[0] = t/2 + tmp;
  ev[1] = t/2 - tmp;

  return ev;
}