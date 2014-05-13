#include "harris.h"
#include "logger.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

bool ApplyKernel(Image* src_img, Image* dst_img, float* kernel, size_t order);
bool Sobel(Image* src_img, Image* dst_img);
double trace(float* m, size_t order);
double det(float* m, size_t order); 

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

  Image* dx_img = imgCreate(w, h, 1);
  Image* dy_img = imgCreate(w, h, 1);

  //Computing the image derivates.
  ApplyKernel(src_img, dx_img, k_dx, 3);
  ApplyKernel(src_img, dy_img, k_dy, 3);

  Image* dxy_img = imgCreate(w, h, 1);

  float* dx_img_data = imgGetData(dx_img);
  float* dy_img_data = imgGetData(dy_img);
  float* dxy_img_data = imgGetData(dxy_img);

  for(int i = 0; i < img_size; i++) {
    dx_img_data[i] *= dx_img_data[i];
    dy_img_data[i] *= dy_img_data[i];
    dxy_img_data[i] = dx_img_data[i] * dy_img_data[i];
  }

  Image* dx_img_gauss = imgCreate(w, h, 1);
  Image* dy_img_gauss = imgCreate(w, h, 1);
  Image* dxy_img_gauss = imgCreate(w, h, 1);

  ApplyKernel(dx_img, dx_img_gauss, k_gauss, 5);
  ApplyKernel(dy_img, dy_img_gauss, k_gauss, 5);
  ApplyKernel(dxy_img, dxy_img_gauss, k_gauss, 5);

  dx_img_data = imgGetData(dx_img_gauss);
  dy_img_data = imgGetData(dy_img_gauss);
  dxy_img_data = imgGetData(dxy_img_gauss);

  glm::mat2* harris_mat = new glm::mat2[img_size];

  for(int i = 0; i < img_size; i++) {
    harris_mat[i][0][0] = dx_img_data[i];
    harris_mat[i][0][1] = dxy_img_data[i];
    harris_mat[i][1][0] = dxy_img_data[i];
    harris_mat[i][1][1] = dy_img_data[i];
  }

  float* dst_data = imgGetData(dst_img);

  double* r = new double[img_size];
  for(int i = 0; i < img_size; i++) {
    double t = trace(glm::value_ptr(harris_mat[i]), 2);
    double d = glm::determinant(harris_mat[i]);

  //  /*printf("%f %f\n%f %f\n", harris_mat[i][0][0], harris_mat[i][0][1], harris_mat[i][1][0], harris_mat[i][1][1]);
  //  printf("%lf %lf\n\n", d, 0.04 * t * t);*/
    r[i] = d - (0.004 * t * t);
    dst_data[i] = r[i];
  }

  imgDestroy(dx_img);
  imgDestroy(dy_img);
  imgDestroy(dxy_img);
  imgDestroy(dx_img_gauss);
  imgDestroy(dy_img_gauss);
  imgDestroy(dxy_img_gauss);
  delete[] r;
  delete[] harris_mat;
  return true;
}

bool ApplyKernel(Image* src_img, Image* dst_img, float* kernel, size_t order)
{
  float* src_data = imgGetData(src_img);
  float* dst_data = imgGetData(dst_img);
  int w = imgGetWidth(src_img);
  int h = imgGetHeight(src_img);

  int kernel_size = order * order;
  int limit = (int)ceil(order / 2);

  for(size_t i = limit; i < h - limit; i++) {
    for(size_t j = limit; j < w - limit; j++) {

      float tmp = 0;

      for(int k = -limit; k <= limit; k++) {
        for(int l = -limit; l <= limit; l++) {
          tmp += kernel[(k + limit) * order + (l + limit)] * src_data[(i + k) * w + (j + l)];
        }
      }

      /*float tmp = kernel[0] * src_data[(i-1) * w + (j-1)];
      tmp += kernel[1] * src_data[(i-1) * w + j];
      tmp += kernel[2] * src_data[(i-1) * w + (j+1)];

      tmp += kernel[3] * src_data[i * w + (j-1)];
      tmp += kernel[4] * src_data[i * w + j];
      tmp += kernel[5] * src_data[i * w + (j+1)];

      tmp += kernel[6] * src_data[(i+1) * w + (j-1)];
      tmp += kernel[7] * src_data[(i+1) * w + j];
      tmp += kernel[8] * src_data[(i+1) * w + (j+1)];*/

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

  ApplyKernel(src_img, dx_img, k_dx, 3);
  ApplyKernel(src_img, dy_img, k_dy, 3);

  float* dx_img_data = imgGetData(dx_img);
  float* dy_img_data = imgGetData(dy_img);
  float* dst_img_data = imgGetData(dst_img);

  for(int i = 0; i < w * h; i++) {
    dst_img_data[i] = sqrt(dx_img_data[i] * dx_img_data[i] + dy_img_data[i] * dy_img_data[i]);
  }

  return true;
}

double trace(float* m, size_t order) {

  size_t s = order * order;
  double t = 0.0;

  for(int i = 0; i < s; i += (order + 1))
    t += m[i];

  return t;
}

double det(float* m, size_t order)
{
  return 0.0;
}