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
  m_intCamMatrix.release();
  m_distCoeff.release();
  for(size_t i = 0; i < m_inputPatt.size(); i++)
    m_inputPatt[i].release();
  m_inputPatt.clear();
}

double Calibration::runCalibration()
{
  vector< vector<Point2f> > corner_points(m_inputPatt.size());
  vector< vector<Point3f> > obj_points(m_inputPatt.size());

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
                               m_rvecs,
                               m_tvecs);

  //Converting the rotation and translation vectors into a modelview matrix
  //to use with OpenGL.
  cout << "--------------------------------------------\n";
  for(size_t i = 0; i < m_rvecs.size(); i++) {
    Mat R;
    Rodrigues(m_rvecs[i], R);

    Mat t =  m_tvecs[i];

    Mat M = Mat::zeros(3, 4, R.type());
    R.copyTo(M.colRange(0, 3).rowRange(0, 3));
    t.copyTo(M.colRange(3, 4).rowRange(0, 3));

    m_mvpMatrices.push_back(M);
  }
  cout << "--------------------------------------------\n\n";

//  cout << "Calibration test:\n";
//  cout << m_intCamMatrix << endl << endl;
//  cout << m_distCoeff << endl << endl;

//  Mat R(4, 4, CV_64F);
//  Rodrigues(rvecs[0], R);

//  cout << "Rodrigues matrix:\n" << R << endl << endl;
//  cout << "Camera position:\n" << tvecs[0] << endl << endl;

  return rpe;
}

Mat Calibration::getOrthoMatrix(float l, float r, float b, float t, float n, float f)
{
  Mat ndc = Mat::eye(4, 4, m_intCamMatrix.type());
  ndc.col(0).row(0) = 2 / (r - l);
  ndc.col(1).row(1) = 2 / (t - b);
  ndc.col(2).row(2) = -2 / (f - n);

  ndc.col(3).row(0) = -(r + l) / (r - l);
  ndc.col(3).row(1) = -(t + b) / (t - b);
  ndc.col(3).row(2) = -(f + n) / (f - n);

  return ndc;
}

//Since OpenCV uses a different set of axes from OpenGL, we need to negate
//the second and third columns of K. This will force the Y axis to point up
//(like in OpenGL) instead of down(like in OpenCV) and the camera wil look
//down upon the Z axis(like OpenGL) instead of the positive Z axis(OpenCV).
//Reference: http://ksimek.github.io/2012/08/14/decompose/

//Plus, we need to add another row and column to the matrix to avoid losing
//Z-depth information. This new matrix will come from this:
//[a, s, -x0]
//[0, b, -y0]
//[0, 0,  -1]

//to this:
//[a, s, -x0, 0]
//[0, b, -y0, 0]
//[0, 0,   A, B]
//[0, 0,  -1, 0]

//where A and B are equal to near + far and near * far respectively. a is the
//x-axis focal length, b is the y-axis focal length, s is the world axis skew,
//x0 and y0 are the image origin in camera coordinates.
Mat Calibration::getProjMatrix(Size img_size, float near, float far)
{
  double fy = getIntCamMatrix().at<double>(4);
  double fovy = 2 * atan(img_size.height / (2 * fy)) * 180.0 / CV_PI;
  glm::mat4 test = glm::perspective((float)fovy, (float)img_size.height / (float)img_size.width, near, far);;

  Mat proj = Mat::zeros(4, 4, CV_64F);

  for(size_t r = 0; r < 3; r++)
    for(size_t c = 0; c < 3; c++)
      proj.at<double>(r, c) = m_intCamMatrix.at<double>(r, c);

  proj.at<double>(2, 2) = near + far;
  proj.at<double>(2, 3) = near * far;
  proj.at<double>(3, 2) = -1;
  proj.at<double>(0, 2) = -proj.at<double>(0, 2);
  proj.at<double>(1, 2) = -proj.at<double>(1, 2);

  proj = getOrthoMatrix(0, img_size.width, 0, img_size.height, near, far) * proj;

  return proj;
}

Mat Calibration::getViewMatrix(size_t idx)
{
  Mat rotation, view_mat(4, 4, CV_64F);
  Rodrigues(m_rvecs[idx], rotation);

  for(size_t r = 0; r < 3; r++) {
    for(size_t c = 0; c < 3; c++)
      view_mat.at<double>(r, c) = rotation.at<double>(r, c);
    view_mat.at<double>(r, 3) = m_tvecs[idx].at<double>(r, 0);
  }
  view_mat.at<double>(3, 3) = 1.0f;

  Mat cvToGl = Mat::eye(4, 4, CV_64F);
  cvToGl.at<double>(1, 1) = -1.0f; // Invert the y axis
  cvToGl.at<double>(2, 2) = -1.0f; // invert the z axis
  view_mat = cvToGl * view_mat;
  
  return view_mat;
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
