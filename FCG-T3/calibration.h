#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace std;
using namespace cv;

class Calibration
{
public:
  Calibration();
  Calibration(vector<string> img_paths);
  ~Calibration();

  double runCalibration();

  Mat getIntCamMatrix() {
    return Mat(m_intCamMatrix);
  }

  Mat getDistCoeffs() {
    return Mat(m_distCoeff);
  }

  Mat getIntCamMatrixOpenGL(float near, float far);
  Mat getOrthoMatrix(float l, float r, float b, float t, float n, float f);
  Mat getProjMatrixGL(float l, float r, float b, float t, float n, float f);
  void getMVPMatrixGL(float l, float r, float b, float t, float n, float f);

  glm::quat getRotationQuat(size_t idx);

  //temporary variables.
  vector<Mat> m_mvpMatrices;

private:
  vector<Mat> m_inputPatt;  //Input patterns.
  Mat m_intCamMatrix;       //Intrinsic camera matrix.
  Mat m_distCoeff;          //Distortion coefficients.

  bool getChessboardCorners(Mat &chess_patt, vector<Point2f>& corners, Size board_size);
  void getObjSpacePoints(vector<Point3f>& obj_space_pts, Size board_size, float square_size);
};

#endif // CALIBRATION_H
