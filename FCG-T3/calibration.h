#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>

using namespace std;
using namespace cv;

class Calibration
{
public:
  Calibration();
  Calibration(vector<string> img_paths);
  ~Calibration();

  double runCalibration();

  vector<Mat> getInputPatterns() {
    return m_inputPatt;
  }

  Mat getIntCamMatrix() {
    return Mat(m_intCamMatrix);
  }

  Mat getDistCoeffs() {
    return Mat(m_distCoeff);
  }

  Mat getOrthoMatrix(float l, float r, float b, float t, float n, float f);
  Mat getProjMatrix(Size img_size, float near, float far);
  Mat getViewMatrix(size_t idx);

  //temporary variables.
  vector<Mat> m_mvpMatrices;
  vector<Mat> m_rvecs;
  vector<Mat> m_tvecs;

private:
  vector<Mat> m_inputPatt;  //Input patterns.
  Mat m_intCamMatrix;       //Intrinsic camera matrix.
  Mat m_distCoeff;          //Distortion coefficients.

  bool getChessboardCorners(Mat &chess_patt, vector<Point2f>& corners, Size board_size);
  void getObjSpacePoints(vector<Point3f>& obj_space_pts, Size board_size, float square_size);
};

#endif // CALIBRATION_H
