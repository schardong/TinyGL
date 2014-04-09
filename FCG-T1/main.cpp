#include "config.h"
#include "tinygl.h"
#include "logger.h"
#include "shader.h"
#include "mesh.h"
#include "grid.h"
#include "sphere.h"
#include "axis.h"
#include "ciemesh.h"
#include "ciepointcloud.h"
#include "colorspace.h"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <string>
#include <vector>

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

extern "C" {
#include "color.h"
#include "mtwist.h"
}

using namespace std;

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

typedef enum { XYZ, RGB, n_colorspaces } colorspaces;

int g_window = -1;

glm::mat4 viewMatrix;
glm::mat4 projMatrix;
glm::vec3 g_eye;
glm::vec3 g_center;

bool initCalled = false;
bool initGLEWCalled = false;

bool g_wireRender = false;
bool g_meshRender = false;
int g_cloudRender = XYZ;

void drawPointsArrays(size_t num_points)
{
  glDrawArrays(GL_POINTS, 0, num_points);
}

void drawLinesIdx(size_t num_points)
{
  glDrawElements(GL_TRIANGLES, num_points, GL_UNSIGNED_SHORT, NULL);
}

void drawAxis(size_t num_points)
{
  glDrawArrays(GL_LINES, 0, num_points);
}

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
  glutInitWindowSize(800, 600);

  g_window = glutCreateWindow("FCG-T1");
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
  glPointSize(2);

  initGLEWCalled = true;
}

int createBetaCurve(float* beta, size_t n)
{
  if (beta == NULL || n == 0)
    return 0;

  for (size_t i = 0; i < n; i++) {
    mts_goodseed(&mt_default_state);
    beta[i] = (float)mt_drand();
  }
  return 1;
}

void createAndWriteBeta(size_t num_samples, size_t delta)
{
  std::string filename = "beta_reflectance_" + std::to_string(num_samples) + "_" + std::to_string(delta) + ".dat";
  FILE* fp = fopen(filename.c_str(), "wb+");

  float* beta = new float[400 / delta];

  for (size_t i = 0; i < num_samples; i++) {
    createBetaCurve(beta, 400 / delta);
    if (i % 100 == 0)
      Logger::getInstance()->log(std::to_string(i) + " beta spectrum");
    fwrite(beta, sizeof(float), 400 / delta, fp);
  }

  fclose(fp);
  delete[] beta;
}

void init()
{
  const int STEP = 1;
  g_eye = glm::vec3(2.5, 2.5, 2.5);
  g_center = glm::vec3(0, 0, 0);
  viewMatrix = glm::lookAt(g_eye, g_center, glm::vec3(0, 1, 0));
  projMatrix = glm::perspective(static_cast<float>(M_PI / 4.f), 1.f, 0.1f, 1000.f);

  std::vector<glm::vec3> xyz;
  std::vector<glm::vec3> xyz_mesh;
  std::vector<glm::vec3> rgb;
  std::vector<glm::vec3> rgb_mesh;

  std::vector<CIEPointCloud*> cieclouds(n_colorspaces);
  std::vector<CIEMesh*> ciemesh(n_colorspaces);
  Axis* axis;

  float* beta = new float[100000 * 400];
  float* illum = new float[400];
  std::vector<glm::vec3> xyzbar;

  for (int i = 0; i < 400; i++)  {
    float x, y, z;
    illum[i] = corGetD65(380.f + i);
    corGetCIExyz(380.f + i, &x, &y, &z);
    xyzbar.push_back(glm::vec3(x, y, z));
  }

  glm::mat3 m = glm::inverse(glm::mat3({ 0.490, 0.310, 0.200, 0.177, 0.813, 0.011, 0.000, 0.010, 0.990 }));

  FILE* fp = fopen("beta_reflectance_100000_1.dat", "rb");
  fread(beta, sizeof(float), 100000 * 400, fp);

  for (size_t i = 0; i < 100000; i++) {
    
    glm::vec3 tmp = createCIEXYZ(beta + i * 400, illum, xyzbar, STEP);
   
    xyz.push_back(tmp);
    rgb.push_back(m * tmp);
  }

  delete[] beta;

  //variar o Y de 0 até 1
  //calcular a curva beta  de forma a montar a superfície do plano
  //feito isso, interpolar os pontos gerados e formar uma malha de triangulos.
  const float Y_STEP = 0.1f;
  float lw[3];
  getReferenceWhite(lw, D65);
    
  for (float Y = Y_STEP; Y < 1.f; Y += Y_STEP) {
    float lws[3];
    lws[0] = Y * lw[0];
    lws[1] = Y * lw[1];
    lws[2] = Y * lw[2];

    for (float lambda = 380.f; lambda < 780.f; lambda += 1.f) {
      float x, y, z;
      float sum = 0.f;
      corGetCIExyz(lambda, &x, &y, &z);
            
      sum = (x + y + z);
      if (sum == 0)
        x = y = z = 0.f;
      else {
        x = lws[0] * x / sum;
        y = lws[1] * y / sum;
        z = lws[2] * z / sum;
      }
     
      glm::vec3 tmp(x, y, z);
      xyz_mesh.push_back(tmp);
      rgb_mesh.push_back(m * tmp);
    }
  }
  xyz_mesh.push_back(glm::vec3(lw[0], lw[1], lw[2]));
  rgb_mesh.push_back(m * glm::vec3(lw[0], lw[1], lw[2]));

  delete[] illum;

  /*for (size_t i = 380; i < 780; i++) {
    float x, y, z;
    memset(beta, 0, sizeof(float)* 400 / STEP);
    beta[i - 380] = 10.f;
    corCIEXYZfromSurfaceReflectance(380.f, 400 / STEP, STEP, beta, &x, &y, &z, D65);

    glm::vec3 tmp(x, y, z);
    xyz.push_back(tmp);
    rgb.push_back(m * tmp);
  }*/
  
  

  /*for (float i = 0; i < 400; i += STEP) {
    float xbar, ybar, zbar;
    float x, y, z;
    float lambda = 380.f + i;

    corGetCIExyz(lambda, &xbar, &ybar, &zbar);

    xbar /= ybar;
    ybar /= ybar;
    zbar /= ybar;

    float sum = (xbar + ybar + zbar);

   
    if (sum == 0) {
      x = y = z = 0.f;
    }
    else {
      x = xbar / sum;
      y = ybar / sum;
      z = zbar / sum;
    }

    glm::vec3 tmp = glm::vec3(x, y, z);
    xyz_mesh.push_back(tmp);
    rgb_mesh.push_back(m * tmp);
  }*/

  cieclouds[XYZ] = new CIEPointCloud(xyz);
  cieclouds[XYZ]->setDrawCb(drawPointsArrays);
  cieclouds[XYZ]->setMaterialColor(glm::vec4(0));
  TinyGL::getInstance()->addMesh("CIExyzCloud", cieclouds[XYZ]);
  cieclouds[XYZ]->m_modelMatrix = glm::mat4(1.f);
  cieclouds[XYZ]->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * cieclouds[XYZ]->m_modelMatrix));

  cieclouds[RGB] = new CIEPointCloud(rgb);
  cieclouds[RGB]->setDrawCb(drawPointsArrays);
  cieclouds[RGB]->setMaterialColor(glm::vec4(0));
  TinyGL::getInstance()->addMesh("CIErgbCloud", cieclouds[RGB]);
  cieclouds[RGB]->m_modelMatrix = glm::mat4(glm::inverse(m));
  cieclouds[RGB]->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * cieclouds[RGB]->m_modelMatrix));

  ciemesh[XYZ] = new CIEMesh(xyz_mesh);
  ciemesh[XYZ]->setDrawCb(drawLinesIdx);
  ciemesh[XYZ]->setMaterialColor(glm::vec4(0));
  TinyGL::getInstance()->addMesh("CIExyzMesh", ciemesh[XYZ]);
  ciemesh[XYZ]->m_modelMatrix = glm::mat4(1.f);
  ciemesh[XYZ]->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * ciemesh[XYZ]->m_modelMatrix));

  ciemesh[RGB] = new CIEMesh(rgb_mesh);
  ciemesh[RGB]->setDrawCb(drawLinesIdx);
  ciemesh[RGB]->setMaterialColor(glm::vec4(0));
  TinyGL::getInstance()->addMesh("CIErgbMesh", ciemesh[RGB]);
  ciemesh[RGB]->m_modelMatrix = glm::mat4(glm::inverse(m));
  ciemesh[RGB]->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * ciemesh[RGB]->m_modelMatrix));

  axis = new Axis(glm::vec2(-1, 1), glm::vec2(-1, 1), glm::vec2(-1, 1));
  axis->setDrawCb(drawAxis);
  axis->setMaterialColor(glm::vec4(0.f));
  TinyGL::getInstance()->addMesh("axis", axis);
  axis->m_modelMatrix = glm::mat4(1.f);
  axis->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * axis->m_modelMatrix));

  Shader* g_shader = new Shader("../Resources/fcgt1.vs", "../Resources/fcgt1.fs");
  g_shader->bind();
  g_shader->bindFragDataLoc("out_vColor", 0);
  g_shader->setUniformMatrix("viewMatrix", viewMatrix);
  g_shader->setUniformMatrix("projMatrix", projMatrix);

  TinyGL::getInstance()->addShader("fcgt1", g_shader);

  initCalled = true;
}

void destroy()
{
  TinyGL::getInstance()->freeResources();
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
  Shader* s = glPtr->getShader("fcgt1");

  s->bind();
  
  switch (g_cloudRender) {
  case XYZ:
    s->setUniformMatrix("modelMatrix", glPtr->getMesh("CIExyzCloud")->m_modelMatrix);
    glPtr->draw("CIExyzCloud");
    if (g_meshRender) {
      s->setUniformMatrix("modelMatrix", glPtr->getMesh("CIExyzMesh")->m_modelMatrix);
      glPtr->draw("CIExyzMesh");
    }
    break;
  case RGB:
    s->setUniformMatrix("modelMatrix", glPtr->getMesh("CIErgbCloud")->m_modelMatrix);
    glPtr->draw("CIErgbCloud");
    if (g_meshRender) {
      s->setUniformMatrix("modelMatrix", glPtr->getMesh("CIErgbMesh")->m_modelMatrix); 
      glPtr->draw("CIErgbMesh");
    }
    break;
  }

  s->setUniformMatrix("modelMatrix", glPtr->getMesh("axis")->m_modelMatrix);
  glPtr->draw("axis");

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
  projMatrix = glm::perspective(static_cast<float>(M_PI / 4.f), static_cast<float>(w) / static_cast<float>(h), 0.1f, 1000.f);

  Shader* s = TinyGL::getInstance()->getShader("fcgt1");
  s->bind();
  s->setUniformMatrix("projMatrix", projMatrix);

  Shader::unbind();
}

void keyPress(unsigned char c, int x, int y)
{
  bool cameraChanged = false;
//  printf("keyPress = %d\n", c);

  switch (c) {
  case 'w':
    g_wireRender = !g_wireRender;
    break;
  case 'm':
    g_meshRender = !g_meshRender;
    break;
  }

  if(g_wireRender)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  else
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  if(cameraChanged) {
    viewMatrix = glm::lookAt(g_eye, g_center, glm::vec3(0, 1, 0));

    Shader* s = TinyGL::getInstance()->getShader("fcgt1");
    s->bind();
    s->setUniformMatrix("viewMatrix", viewMatrix);
  }
}

void specialKeyPress(int c, int x, int y)
{
  bool cameraChanged = false;
//  printf("specialKeyPress = %d\n", c);

  glm::vec3 back = g_eye - g_center;

  switch (c) {
  case GLUT_KEY_LEFT:
    back = glm::mat3(glm::rotate(-(float)M_PI / 100.f, glm::vec3(0, 1, 0))) * back;
    g_eye = back + g_center;
    cameraChanged = true;
    break;
  case GLUT_KEY_RIGHT:
    back = glm::mat3(glm::rotate((float)M_PI / 100.f, glm::vec3(0, 1, 0))) * back;
    g_eye = back + g_center;
    cameraChanged = true;
    break;
  case GLUT_KEY_UP:
    back = glm::mat3(glm::rotate(-(float)M_PI / 100.f, glm::vec3(1, 0, 0))) * back;
    g_eye = back + g_center;
    cameraChanged = true;
    break;
  case GLUT_KEY_DOWN:
    back = glm::mat3(glm::rotate((float)M_PI / 100.f, glm::vec3(1, 0, 0))) * back;
    g_eye = back + g_center;
    cameraChanged = true;
    break;
  case GLUT_KEY_F1:
    g_cloudRender = XYZ;
    break;
  case GLUT_KEY_F2:
    g_cloudRender = RGB;
    break;
  }

  if(cameraChanged) {
    viewMatrix = glm::lookAt(g_eye, g_center, glm::vec3(0, 1, 0));

    Shader* s = TinyGL::getInstance()->getShader("fcgt1");
    s->bind();
    s->setUniformMatrix("viewMatrix", viewMatrix);
  }
}

void exit_cb()
{
  glutDestroyWindow(g_window);
  destroy();
  exit(EXIT_SUCCESS);
}
