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

glm::mat4 g_projMatrix;
glm::mat4 g_modelView;

Calibration* g_calib;

GLuint g_patternsTex[NUM_IMAGES];
GLuint g_cornersTex[NUM_IMAGES];
GLuint g_patternIdx = 0;

bool initCalled = false;
bool initGLEWCalled = false;

void initPatternsCV();
void setupMeshes();
void setupShaders();
void resendShaderUniforms();
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
  printInstructions();

  setupMeshes();

  vector<string> patt_path;

  for(int i = 0; i < NUM_IMAGES / 2; i++) {
    string prefix = "../Resources/images/left";
    if(i < 9) prefix += "0";
    patt_path.push_back(string(prefix + to_string(i+1) + ".bmp"));
  }

  for(int i = NUM_IMAGES / 2; i < NUM_IMAGES; i++) {
    string prefix = "../Resources/images/right";
    if((i - (NUM_IMAGES / 2)) < 9) prefix += "0";
    patt_path.push_back(string(prefix + to_string(i - (NUM_IMAGES/2) + 1) + ".bmp"));
  }

  g_calib = new Calibration(patt_path);
  double rpe = g_calib->runCalibration();

  g_projMatrix = g_calib->getProjMatrix(Size(640, 480), 1.f, 30.f);

  setupShaders();
  resendShaderUniforms();
  initPatternsCV();

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

  TinyGL* glPtr = TinyGL::getInstance();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  Shader* s = glPtr->getShader("simple");
  s->bind();
  s->setUniformMatrix("MV", g_modelView);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  Mesh* m = glPtr->getMesh("sphere");
  m->bind();
  m->draw();

  s = glPtr->getShader("square");

  m = glPtr->getMesh("quad");
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  s->bind();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, g_patternsTex[g_patternIdx]);

  //s->setUniformMatrix("modelMatrix", m->m_modelMatrix);
  //m->draw();

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

  string title = WINDOW_TITLE + " pattern";

  resendShaderUniforms();

  if(g_patternIdx < 10) title += "0";
  title += to_string(g_patternIdx + 1);
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
  vector<Mat> patterns = g_calib->getInputPatterns();
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
  glBindTexture(GL_TEXTURE_2D, 0);
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
  printf("O programa inicia carregando as imagens de tabuleiros de xadrex localizadas em ../Resources/images/*.bmp\n");
  printf("O programa usa as varias imagens de tabuleiros de xadrez para calibrar a camera.\n");
  printf("Essa calibracao nos da duas matrizes, a de parametros intrinsicos e extrinsicos.\n");
  printf("Para trocar a imagem exibida, aperte um numero [1,9].\n");
  printf("Para trocar o modo de exibicao da imagem original para a imagem que exibe os cantos detectados, aperte barra de espaco.\n");
  printf("Aperte F10 para exibir essas instrucoes novamente.\n");
  printf("---------------------------------------------------------------\n");
}

void setupMeshes()
{
  Quad* q = new Quad();
  q->setDrawCb(drawQuad);
  q->setMaterialColor(glm::vec4(1.f));
  q->m_modelMatrix = glm::mat4(1.f);
  TinyGL::getInstance()->addResource(MESH, "quad", q);

  Sphere* sph = new Sphere(32, 32);
  sph->setDrawCb(drawSphere);
  sph->setMaterialColor(glm::vec4(0, 0, 1, 0));
  sph->m_modelMatrix = glm::mat4(1.f);
  sph->m_normalMatrix = glm::mat3(1.f);
  TinyGL::getInstance()->addResource(MESH, "sphere", sph);
}

void setupShaders()
{
  Shader* square = new Shader("../Resources/shaders/fcgt2.vs", "../Resources/shaders/fcgt2.fs");
  square->bind();
  square->bindFragDataLoc("fColor", 0);
  square->setUniform1i("u_image", 0);
  TinyGL::getInstance()->addResource(SHADER, "square", square);
  
  Shader* simple = new Shader("../Resources/shaders/fcgt3.vs", "../Resources/shaders/fcgt3.fs");
  simple->bind();
  simple->bindFragDataLoc("fColor", 0);
  simple->setUniformMatrix("u_projMatrix", g_projMatrix);
  TinyGL::getInstance()->addResource(SHADER, "simple", simple);
}

void resendShaderUniforms()
{
  g_modelView = glm::mat4(1.f);
  
  Mat mv = g_calib->getViewMatrix(g_patternIdx);
  
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      g_modelView[j][i] = mv.at<double>(i, j);

  Shader* s = TinyGL::getInstance()->getShader("simple");
  s->bind();
  s->setUniform4fv("u_materialColor", TinyGL::getInstance()->getMesh("sphere")->getMaterialColor());
  s->setUniformMatrix("MV", g_modelView);
  Shader::unbind();
}