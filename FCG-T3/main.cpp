#include "tglconfig.h"
#include "fcgt3config.h"
#include "tinygl.h"
#include "logger.h"
#include "shader.h"
#include "quad.h"
#include "sphere.h"
#include "cube.h"
#include "calibration.h"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <string>
#include <vector>

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>
#include <opencv2/calib3d/calib3d.hpp>

using namespace std;
using namespace cv;

void init();
void initGLUT(int argc, char** argv);
void initGLEW();
void destroy();
void update();
void draw();
void reshape(int w, int h);
void keyPress(unsigned char c, int, int);
void specialKeyPress(int c, int x, int y);
void exit_cb();
void printInstructions();

int g_window = -1;

glm::mat4 viewMatrix;
glm::mat4 projMatrix;
glm::vec3 g_eye;
glm::vec3 g_center;

Calibration* g_calib;

GLuint g_patternsTex[NUM_IMAGES];
GLuint g_cornersTex[NUM_IMAGES];
GLuint g_patternIdx = 0;
bool g_showCorner = true;

bool initCalled = false;
bool initGLEWCalled = false;

void initPatternsCV();
void drawQuad(size_t num_points);
void drawSphere(size_t num_points);

int main(int argc, char** argv)
{
  Logger::getInstance()->setLogStream(&cout);
  Logger::getInstance()->log(TINYGL_LIBNAME + string(" v") + to_string(TINYGL_MAJOR_VERSION) + "." + to_string(TINYGL_MINOR_VERSION));

  initGLUT(argc, argv);
  initGLEW();
  init();

  glutMainLoop();
  return 0;
}

void initGLUT(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
  glutInitWindowSize(WINDOW_W, WINDOW_H);

  g_window = glutCreateWindow(WINDOW_TITLE.c_str());
  glutReshapeFunc(reshape);
  glutDisplayFunc(update);
  glutKeyboardFunc(keyPress);
  glutSpecialFunc(specialKeyPress);

  glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
  glutCloseFunc(exit_cb);
  glutSetKeyRepeat(GLUT_KEY_REPEAT_ON);
}

void initGLEW()
{
  GLenum err = glewInit();

  if (err != GLEW_OK) {
    Logger::getInstance()->error("Failed to initialize GLEW");
    std::cerr << glewGetErrorString(err) << std::endl;
    exit(1);
  }

  glClearColor(0.8f, 0.8f, 0.8f, 1.f);
  glEnable(GL_DEPTH_TEST);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  //glPointSize(2);

  initGLEWCalled = true;
}

void init()
{
  g_eye = glm::vec3(0.0, 0.0, 2.0);
  g_center = glm::vec3(0, 0, 0);
  viewMatrix = glm::lookAt(g_eye, g_center, glm::vec3(0, 1, 0));
  projMatrix = glm::perspective(45.f, 1.f, 1.f, 5.f);

  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++)
      cout << viewMatrix[i][j] << " ";
    cout << endl;
  }

  cout << endl;

  Quad* q = new Quad();
  q->setDrawCb(drawQuad);
  q->setMaterialColor(glm::vec4(1.f));
  q->m_modelMatrix = glm::mat4(1.f);
  q->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * q->m_modelMatrix));
  TinyGL::getInstance()->addResource(MESH, "quad", q);

  Sphere* sph = new Sphere(80, 80);
  sph->setDrawCb(drawSphere);
  sph->setMaterialColor(glm::vec4(0, 0, 1, 0));
  sph->m_modelMatrix = glm::mat4(1.f);
  sph->m_normalMatrix = glm::mat3(1.f);
  TinyGL::getInstance()->addResource(MESH, "sphere", sph);

  printInstructions();
  initPatternsCV();

  vector<string> patt_path;

  for(int i = 0; i < NUM_IMAGES / 2; i++) {
    string prefix = "../../../Resources/images/left";
    if(i < 9) prefix += "0";
    patt_path.push_back(string(prefix + to_string(i+1) + ".bmp"));
  }

  for(int i = NUM_IMAGES / 2; i < NUM_IMAGES; i++) {
    string prefix = "../../../Resources/images/right";
    if((i - (NUM_IMAGES / 2)) < 9) prefix += "0";
    patt_path.push_back(string(prefix + to_string(i - (NUM_IMAGES/2) + 1) + ".bmp"));
  }


  g_calib = new Calibration(patt_path);
  double rpe = g_calib->runCalibration();

  if(rpe > 1.f) {
    Logger::getInstance()->error("Reprojection error larger than acceptable. " + to_string(rpe));
  }

  //Mat proj_cv = g_calib->getProjMatrixGL(0, 640, 0, 480, 1, 5000);
  //cout << "Final ndc * proj matrix:\n" << proj_cv << endl << endl;

//  for(int i = 0; i < 4; i++)
//    for(int j = 0; j < 4; j++)
//      projMatrix[i][j] = proj_cv.at<double>(i, j);

  g_calib->getMVPMatrixGL(0, 640, 0, 480, 1, 5000);
  glm::mat4 MVP = glm::mat4(1.f);
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      MVP[i][j] = g_calib->m_mvpMatrices[g_patternIdx].at<double>(i, j);

  Shader* square = new Shader("../../../Resources/shaders/fcgt2.vs", "../../../Resources/shaders/fcgt2.fs");
  square->bind();
  square->bindFragDataLoc("fColor", 0);
  square->setUniform1i("u_image", 0);
  TinyGL::getInstance()->addResource(SHADER, "square", square);

  Shader* simple = new Shader("../../../Resources/shaders/fcgt3.vs", "../../../Resources/shaders/fcgt3.fs");
  simple->bind();
  simple->bindFragDataLoc("fColor", 0);
  simple->setUniformMatrix("MVP", MVP);
  TinyGL::getInstance()->addResource(SHADER, "simple", simple);

  initCalled = true;
}

void destroy()
{
  TinyGL::getInstance()->freeResources();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDeleteTextures(NUM_IMAGES, g_patternsTex);
  glDeleteTextures(NUM_IMAGES, g_cornersTex);
  delete g_calib;
}

void update()
{
  if (!initCalled || !initGLEWCalled)
    return;

  draw();
}

void draw()
{
  if (!initCalled || !initGLEWCalled)
    return;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  TinyGL* glPtr = TinyGL::getInstance();
  Shader* s = glPtr->getShader("square");

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  s->bind();
  glActiveTexture(GL_TEXTURE0);
  if(!g_showCorner)
    glBindTexture(GL_TEXTURE_2D, g_patternsTex[g_patternIdx]);
  else
    glBindTexture(GL_TEXTURE_2D, g_cornersTex[g_patternIdx]);

  s->setUniformMatrix("modelMatrix", glPtr->getMesh("quad")->m_modelMatrix);
  glPtr->getMesh("quad")->draw();


  s = glPtr->getShader("simple");
  s->bind();

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  Mesh* m = glPtr->getMesh("sphere");
  m->bind();
  m->draw();

  glBindVertexArray(0);
  Shader::unbind();

  glutSwapBuffers();
  glutPostRedisplay();
}

void reshape(int w, int h)
{
  if (!initCalled || !initGLEWCalled)
    return;

  glViewport(0, 0, w, h);
  WINDOW_W = w;
  WINDOW_H = h;
}

void keyPress(unsigned char c, int, int)
{
  int t = c - 49;
  if(t >= 0 && t <= 9) {
    if(t < NUM_PATTERNS)
      g_patternIdx = t;
  }

  switch (c) {
  case ' ':
    g_showCorner = !g_showCorner;
    break;
  default:
    //printf("(%d, %d) = %d, %c\n", x, y, c, c);
    break;
  }

  string title = WINDOW_TITLE + " pattern";

  glm::mat4 MVP = glm::mat4(1.f);
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      MVP[i][j] = g_calib->m_mvpMatrices[g_patternIdx].at<double>(i, j);

  Shader* s = TinyGL::getInstance()->getShader("simple");
  s->bind();
  s->setUniformMatrix("MVP", MVP);
  Shader::unbind();

  if(g_patternIdx < 10) title += "0";
  title += to_string(g_patternIdx + 1);
  if(g_showCorner) title += " (corners)";
  glutSetWindowTitle((title).c_str());
}

void specialKeyPress(int c, int x, int y)
{
  switch (c) {
  case GLUT_KEY_F10:
    printInstructions();
    break;
  default:
    printf("(%d, %d) = %d\n", x, y, c);
    break;
  }

  string title = WINDOW_TITLE + " pattern";

  if(g_patternIdx < 10) title += "0";
  title += to_string(g_patternIdx + 1);
  if(g_showCorner) title += " (corners)";
  glutSetWindowTitle((title).c_str());
}

void exit_cb()
{
  glutDestroyWindow(g_window);
  destroy();
  exit(EXIT_SUCCESS);
}

void initPatternsCV()
{
  Logger* log = Logger::getInstance();
  log->log("Initializing the patterns.");

  vector<Mat> patterns(NUM_IMAGES);

  //Reading the chessboard patterns.
  for(int i = 0; i < NUM_IMAGES / 2; i++) {
    string prefix = "../../../Resources/images/left";
    if(i < 9) prefix += "0";
    patterns[i] = imread(prefix + to_string(i+1) + ".bmp", CV_LOAD_IMAGE_GRAYSCALE);
    log->log("Loaded " + prefix + to_string(i+1) + ".bmp");
  }

  for(int i = NUM_IMAGES / 2; i < NUM_IMAGES; i++) {
    string prefix = "../../../Resources/images/right";
    if((i - (NUM_IMAGES / 2)) < 9) prefix += "0";
    patterns[i] = imread(prefix + to_string(i - (NUM_IMAGES/2) + 1) + ".bmp", CV_LOAD_IMAGE_GRAYSCALE);
    log->log("Loaded " + prefix + to_string(i - (NUM_IMAGES/2) + 1) + ".bmp");
  }

  log->log("Done reading the input images.");

  //Detecting the chessboard corners.
//  vector< vector<Point2f> > corner_values(NUM_IMAGES);
//  vector< vector<Point3f> > obj_space_points(NUM_IMAGES);
//  vector<Mat> corners(NUM_IMAGES);

//  for(int i = 0; i < NUM_IMAGES; i++) {
//    log->log("Finding the chessboard corners on the image " + to_string(i + 1));
//    Size s(7, 6);
//    corners[i] = Mat::zeros(patterns[i].size(), CV_32FC1);

//    bool found = findChessboardCorners(patterns[i], s, corner_values[i], CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE | CV_CALIB_CB_FAST_CHECK);

//    if(found) {
//      cornerSubPix(patterns[i], corner_values[i], Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

//      for(size_t j = 0; j < corner_values[i].size(); j++)
//        corners[i].at<float>(corner_values[i][j]) = 1.f;

//      log->log(to_string(corner_values[i].size()) + " chessboard corners found.");

//      //Creating the object space coordinate points for camera calibration.
//      for(size_t j = 0; j < corner_values[i].size(); j++)
//        obj_space_points[i].push_back(Point3f(corner_values[i][j].x / s.width, corner_values[i][j].y / s.height, 0));

//    }
    
//  }

//  //Reducing the vector size if one of the patterns was not found.
//  for(size_t i = 0; i < corner_values.size();) {
//    if(corner_values[i].empty() || obj_space_points[i].empty()) {
//      corner_values.erase(corner_values.begin() + i);
//      obj_space_points.erase(obj_space_points.begin() + i);
//    } else {
//      i++;
//    }
//  }

//  log->log("Detection complete. Initializing calibration parameters.");
//  //Initializing the camera matrix. Horizontal and vertical aspect ratios are assumed 1.
//  Mat cam_matrix;
//  Mat dist_coeffs = Mat::zeros(8, 1, CV_64F);
//  vector<Mat> rvecs;
//  vector<Mat> tvecs;

//  cam_matrix = Mat::eye(3, 3, CV_64F);
//  cam_matrix.ptr<double>(0)[0] = 1;
//  cam_matrix.ptr<double>(1)[1] = 1;

//  //Calibrating.
//  log->log("Calibration starting.");
//  double rpe = calibrateCamera(obj_space_points, corner_values, patterns[0].size(), cam_matrix, dist_coeffs, rvecs, tvecs);
//  log->log("Calibration finished. Reprojection error = " + to_string(rpe));

//  Mat R;
//  Rodrigues(rvecs[0], R);

////  cout << "Rodrigues matrix:\n";
////  cout << R << endl << endl;

//  R = R.t();

////  cout << "Transposed Rodrigues matrix:\n";
////  cout << R << endl << endl;

//  Mat t = -R * tvecs[0];

////  cout << "Translation vector:\n";
////  cout << t << endl << endl;

//  Mat T = Mat::eye(4, 4, R.type());

//  R.copyTo(T.colRange(0, 3).rowRange(0, 3));
//  t.copyTo(T.colRange(3, 4).rowRange(0, 3));

////  double* p = T.ptr<double>(3);
////  p[0] = p[1] = p[2] = 0;
////  p[3] = 1;

//  cout << T << endl;

//  cout << "Camera matrix:\n" << cam_matrix << endl;
//  cout << "Distortion coefficients:\n" << dist_coeffs << endl;

//  /*------------------TESTS------------------*/
////  vector<Mat> rvecs_test(obj_space_points.size());
////  vector<Mat> tvecs_test(obj_space_points.size());

////  log->log("Begining the solvePnP calls.");
////  for(size_t i = 0; i < obj_space_points.size(); i++) {
////    bool found = solvePnP(obj_space_points[i], corner_values[i], cam_matrix, dist_coeffs, rvecs_test[i], tvecs_test[i]);

////    if(found) {
////      cv::Mat diff = rvecs_test[i] != rvecs[i];
////      bool eq = cv::countNonZero(diff) == 0;
////      if(eq)
////        cout << "Not the same rotation vector!\n" << rvecs_test[i] << endl << rvecs[i] << endl;

////      diff = tvecs_test[i] != tvecs[i];
////      eq = cv::countNonZero(diff) == 0;
////      if(eq)
////        cout << "Not the same translation vector!\n" << tvecs_test[i] << endl << tvecs[i] << endl;
////    }
////  }

//  /*for(size_t i = 0; i < obj_space_points[0].size(); i++) {
//    cout << "(" << obj_space_points[0][i].x << ", " << obj_space_points[0][i].y << ", " << obj_space_points[0][i].z << ")\t";
//    cout << "(" << corner_values[0][i].x << ", " << corner_values[0][i].y << ")\n";
//  }*/

  //Creating the textures to show the results.
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(NUM_IMAGES, g_patternsTex);
  for(int i = 0; i < NUM_IMAGES; i++) {
    int w = patterns[i].cols;
    int h = patterns[i].rows;
    uchar* pattern_data = patterns[i].data;
    glBindTexture(GL_TEXTURE_2D, g_patternsTex[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, pattern_data);
  }

  //Same as above.
//  glGenTextures(NUM_IMAGES, g_cornersTex);
//  for(int i = 0; i < NUM_IMAGES; i++) {
//    int w = corners[i].cols;
//    int h = corners[i].rows;
//    float* corner_data = (float*)corners[i].data;
//    glBindTexture(GL_TEXTURE_2D, g_cornersTex[i]);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, GL_FLOAT, corner_data);
//  }
  glBindTexture(GL_TEXTURE_2D, 0);

  glutReshapeWindow(patterns[0].cols, patterns[0].rows);
}

void drawQuad(size_t num_points)
{
  glDrawElements(GL_TRIANGLE_STRIP, num_points, GL_UNSIGNED_BYTE, NULL);
}

void drawSphere(size_t num_points)
{
  glDrawElements(GL_TRIANGLES, num_points, GL_UNSIGNED_INT, NULL);
}

void printInstructions()
{
  printf("---------------------------------------------------------------\n");
  printf("O programa inicia carregando as imagens de tabuleiros de xadrex localizadas em ../../../Resources/images/*.bmp\n");
  printf("O programa usa as varias imagens de tabuleiros de xadrez para calibrar a camera.\n");
  printf("Essa calibracao nos da duas matrizes, a de parametros intrinsicos e extrinsicos.\n");
  printf("Para trocar a imagem exibida, aperte um numero [1,9].\n");
  printf("Para trocar o modo de exibicao da imagem original para a imagem que exibe os cantos detectados, aperte barra de espaco.\n");
  printf("Aperte F10 para exibir essas instrucoes novamente.\n");
  printf("---------------------------------------------------------------\n");
}
