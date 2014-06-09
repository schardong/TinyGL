#include "calibration.h"

using namespace std;
using namespace cv;

Calibration::Calibration()
{
}

Calibration::Calibration(vector<string> img_paths)
{
  for(size_t i = 0; i < img_paths.size(); i++)
    m_inputPatt.push_back(imread(img_paths[i], CV_LOAD_IMAGE_GRAYSCALE));

  m_intCamMatrix = Mat::eye(3, 3, CV_64F);
  m_distCoeff = Mat::zeros(8, 1, CV_64F);
}

Calibration::~Calibration()
{
}

double Calibration::runCalibration()
{
  vector< vector<Point2f> > corner_points(m_inputPatt.size());
  vector< vector<Point3f> > obj_points(m_inputPatt.size());
  vector<Mat> rvecs;
  vector<Mat> tvecs;

  for(size_t i = 0; i < m_inputPatt.size(); i++) {
    Size s(7, 6);
    bool found = getChessboardCorners(m_inputPatt[i], corner_points[i], s);
    if(found)
      getObjSpacePoints(obj_points[i], s, 1.f);
  }

  for(size_t i = 0; i < corner_points.size();) {
    if(corner_points[i].empty() || obj_points[i].empty()) {
      corner_points.erase(corner_points.begin() + i);
      obj_points.erase(obj_points.begin() + i);
    } else
      i++;
  }

  double rpe = calibrateCamera(obj_points,
                               corner_points,
                               m_inputPatt[0].size(),
                               m_intCamMatrix,
                               m_distCoeff,
                               rvecs,
                               tvecs);

  cout << "Calibration test:\n";
  cout << m_intCamMatrix << endl << endl;
  cout << m_distCoeff << endl << endl;

  return rpe;
}

bool Calibration::getChessboardCorners(Mat& chess_patt, vector<Point2f> &corners, Size board_size)
{
  bool found = findChessboardCorners(chess_patt, board_size, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
  if(found) {
    cornerSubPix( chess_patt,
                  corners,
                  Size(11,11),
                  Size(-1,-1),
                  TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1 ));
  }

  return found;
}

void Calibration::getObjSpacePoints(vector<Point3f> &obj_space_pts, Size board_size, float square_size)
{
  for(int i = 0; i < board_size.height; i++)
    for(int j = 0; j < board_size.width; j++)
      obj_space_pts.push_back(Point3f(float(j * square_size),
                                      float(i * square_size),
                                      0));
}
