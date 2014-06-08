#include "tglconfig.h"
#include "fcgt3config.h"
#include "tinygl.h"
#include "logger.h"
#include "shader.h"
#include "quad.h"

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
void keyPress(unsigned char c, int x, int y);
void specialKeyPress(int c, int x, int y);
void exit_cb();
void printInstructions();

int g_window = -1;

glm::mat4 viewMatrix;
glm::mat4 projMatrix;
glm::vec3 g_eye;
glm::vec3 g_center;

GLuint g_patternsTex[NUM_IMAGES];
GLuint g_cornersTex[NUM_IMAGES];
GLuint g_patternIdx = 0;
bool g_showCorner = true;

bool initCalled = false;
bool initGLEWCalled = false;

void initPatternsCV();
void drawQuad(size_t num_points);

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

  Quad* q = new Quad();
  q->setDrawCb(drawQuad);
  q->setMaterialColor(glm::vec4(1.f));
  q->m_modelMatrix = glm::mat4(1.f);
  q->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * q->m_modelMatrix));
  TinyGL::getInstance()->addResource(MESH, "quad", q);

  initPatternsCV();
  printInstructions();

  Shader* g_shader = new Shader("../../../Resources/shaders/fcgt2.vs", "../../../Resources/shaders/fcgt2.fs");
  g_shader->bind();
  g_shader->bindFragDataLoc("fColor", 0);
  g_shader->setUniform1i("u_image", 0);
  TinyGL::getInstance()->addResource(SHADER, "fcgt2", g_shader);

  initCalled = true;
}

void destroy()
{
  TinyGL::getInstance()->freeResources();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDeleteTextures(NUM_IMAGES, g_patternsTex);
  glDeleteTextures(NUM_IMAGES, g_cornersTex);
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
  Shader* s = glPtr->getShader("fcgt2");

  s->bind();
  glActiveTexture(GL_TEXTURE0);
  if(!g_showCorner)
    glBindTexture(GL_TEXTURE_2D, g_patternsTex[g_patternIdx]);
  else
    glBindTexture(GL_TEXTURE_2D, g_cornersTex[g_patternIdx]);

  s->setUniformMatrix("modelMatrix", glPtr->getMesh("quad")->m_modelMatrix);
  glPtr->getMesh("quad")->draw();

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

void keyPress(unsigned char c, int x, int y)
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
  for(int i = 0; i < NUM_PATTERNS; i++) {
    string prefix = "../../../Resources/images/left";
    if(i < 10) prefix += "0";
    patterns[i] = imread(prefix + to_string(i+1) + ".bmp", CV_LOAD_IMAGE_GRAYSCALE);
    log->log("Loaded " + prefix + to_string(i+1) + ".bmp");
  }

  log->log("Done reading the input images.");

  //Detecting the chessboard corners.
  vector< vector<Point2f> > corner_values(NUM_IMAGES);
  vector< vector<Point3f> > obj_space_points(NUM_IMAGES);
  vector<Mat> corners(NUM_IMAGES);

  for(int i = 0; i < NUM_IMAGES; i++) {
    log->log("Finding the chessboard corners on the image " + to_string(i + 1));
    Size s(7, 6);
    corners[i] = Mat::zeros(patterns[i].size(), CV_32FC1);

    bool found = findChessboardCorners(patterns[i], s, corner_values[i], CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE | CV_CALIB_CB_FAST_CHECK);

    if(found) {
      cornerSubPix(patterns[i], corner_values[i], Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

      for(size_t j = 0; j < corner_values[i].size(); j++)
        corners[i].at<float>(corner_values[i][j]) = 1.f;

      log->log(to_string(corner_values[i].size()) + " chessboard corners found.");

      //Creating the object space coordinate points for camera calibration.
      for(size_t j = 0; j < corner_values[i].size(); j++)
        obj_space_points[i].push_back(Point3f(corner_values[i][j].x / s.width, corner_values[i][j].y / s.height, 0));
    }
    
  }

  for(size_t i = 0; i < corner_values.size(); i++) {
    if(corner_values[i].empty()) {
      corner_values.erase(corner_values.begin() + i);
      obj_space_points.erase(obj_space_points.begin() + i);
    }
  }

  log->log("Detection complete. Initializing calibration parameters.");
  //Initializing the camera matrix. Horizontal and vertical aspect ratios are assumed 1.
  Mat cam_matrix;
  Mat dist_coeffs = Mat::zeros(8, 1, CV_64F);
  vector<Mat> rvecs;
  vector<Mat> tvecs;

  cam_matrix = Mat::eye(3, 3, CV_64F);
  cam_matrix.ptr<double>(0)[0] = 1;
  cam_matrix.ptr<double>(1)[1] = 1;

  //Calibrating.
  log->log("Calibration starting.");
  double rpe = calibrateCamera(obj_space_points, corner_values, patterns[0].size(), cam_matrix, dist_coeffs, rvecs, tvecs);
  log->log("Calibration finished. Reprojection error = " + to_string(rpe));

  cout << "Camera matrix: " << cam_matrix << endl;
  cout << "Distortion coefficients: " << dist_coeffs << endl;

  /*for(size_t i = 0; i < obj_space_points[0].size(); i++) {
    cout << "(" << obj_space_points[0][i].x << ", " << obj_space_points[0][i].y << ", " << obj_space_points[0][i].z << ")\t";
    cout << "(" << corner_values[0][i].x << ", " << corner_values[0][i].y << ")\n";
  }*/

  /*for(int i = 0; i < rvecs.size(); i++) {
    cout << rvecs[i] << endl;
  }
  cout << endl;

  for(int i = 0; i < tvecs.size(); i++) {
    cout << tvecs[i] << endl;
  }
  cout << endl;*/

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
  glGenTextures(NUM_IMAGES, g_cornersTex);
  for(int i = 0; i < NUM_IMAGES; i++) {
    int w = corners[i].cols;
    int h = corners[i].rows;
    float* corner_data = (float*)corners[i].data;
    glBindTexture(GL_TEXTURE_2D, g_cornersTex[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, GL_FLOAT, corner_data);
  }
  glBindTexture(GL_TEXTURE_2D, 0);

  glutReshapeWindow(patterns[0].cols, patterns[0].rows);
}

void drawQuad(size_t num_points)
{
  glDrawElements(GL_TRIANGLE_STRIP, num_points, GL_UNSIGNED_BYTE, NULL);
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
