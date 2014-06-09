#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>
#include <opencv2/calib3d/calib3d.hpp>

using namespace std;
using namespace cv;

class Calibration
{
public:
  Calibration();
  Calibration(vector<string> img_paths);
  ~Calibration();

  double runCalibration();

private:
  vector<Mat> m_inputPatt;
  Mat m_intCamMatrix; //Intrinsic camera matrix.
  Mat m_distCoeff;    //Distortion coefficients.

  bool getChessboardCorners(Mat &chess_patt, vector<Point2f>& corners, Size board_size);
  void getObjSpacePoints(vector<Point3f>& obj_space_pts, Size board_size, float square_size);
};

#endif // CALIBRATION_H
