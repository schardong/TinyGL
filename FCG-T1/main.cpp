#include "config.h"
#include "tinygl.h"
#include "logger.h"
#include "shader.h"
#include "mesh.h"
#include "grid.h"
#include "sphere.h"
#include "axis.h"
#include "ciexyzmesh.h"

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

int g_window = -1;

CIExyzMesh* ciexyz;
CIExyzMesh* ciergb;
Axis* axis;

glm::mat4 viewMatrix;
glm::mat4 projMatrix;
glm::vec3 g_eye;
glm::vec3 g_center;

bool initCalled = false;
bool initGLEWCalled = false;

bool g_wireRender = false;
bool g_xyzRender = true;

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

void init()
{
  const int STEP = 1;
  g_eye = glm::vec3(1.5, 1.4, 1.5);
  g_center = glm::vec3(0, 0, 0);
  viewMatrix = glm::lookAt(g_eye, g_center, glm::vec3(0, 1, 0));
  projMatrix = glm::perspective(static_cast<float>(M_PI / 4.f), 1.f, 0.1f, 100.f);

  std::vector<glm::vec3> xyz;
  std::vector<glm::vec3> rgb;
  
  float* beta = new float[400 / STEP];

  for (size_t i = 0; i < 400 / STEP; i++) {
    memset(beta, 0, sizeof(float)* 400 / STEP);
    beta[i] = 100.f;

    float x, y, z;
    corGetCIExyfromLambda(i + 380.f, &x, &y);
    
    //corCIEXYZfromSurfaceReflectance(380.f, 400 / STEP, STEP, beta, &x, &y, &z, F11);

    glm::vec3 tmp(x, y, z);
    xyz.push_back(tmp);

    corCIEXYZtoCIERGB(x, y, z, &tmp.x, &tmp.y, &tmp.z);

    rgb.push_back(tmp);
  }

  delete[] beta;


  /*for(float i = 0; i < 400; i += STEP) {
    float xbar, ybar, zbar;
    float x, y, z, A;
    float lambda = 380.f + i;

    corGetCIExyz(lambda, &xbar, &ybar, &zbar);
    A = corGetD65(lambda);
    
    float xyz_sum = (xbar + ybar + zbar);

    if (xyz_sum == 0) {
      x = y = z = 0.f;
    } else {
      x = xbar / xyz_sum;
      y = ybar / xyz_sum;
      z = zbar / xyz_sum;
    }

    glm::vec3 tmp = glm::vec3(x, y, z);
    xyz.push_back(tmp);
    rgb.push_back(m * tmp);
  }*/

  /*printf("[");
  for (size_t i = 0; i < A.size(); i++) {
    printf("%f, ", A[i]);
  }
  printf("]\n\n");*/

  /*printf("[");
  for(size_t i = 0; i < xyz.size(); i++) {
    printf("%f, ", xyz[i].x);
  }
  printf("]\n\n");

  printf("[");
  for(size_t i = 0; i < xyz.size(); i++) {
    printf("%f, ", xyz[i].y);
  }
  printf("]\n\n");

  printf("[");
  for(size_t i = 0; i < xyz.size(); i++) {
    printf("%f, ", xyz[i].z);
  }
  printf("]\n\n");*/

  ciexyz = new CIExyzMesh(xyz);
  ciexyz->setDrawCb(drawLinesIdx);
  ciexyz->setMaterialColor(glm::vec4(0));
  TinyGL::getInstance()->addMesh("CIExyz", ciexyz);
  ciexyz->m_modelMatrix = glm::mat4(1.f);
  ciexyz->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * ciexyz->m_modelMatrix));

  ciergb = new CIExyzMesh(rgb);
  ciergb->setDrawCb(drawLinesIdx);
  ciergb->setMaterialColor(glm::vec4(0));
  TinyGL::getInstance()->addMesh("CIErgb", ciergb);
  ciergb->m_modelMatrix = glm::mat4(1.f);
  ciergb->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * ciergb->m_modelMatrix));

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
  s->setUniformMatrix("modelMatrix", glm::mat4(1.f));

  if(g_xyzRender) {
    glPtr->draw("CIExyz");
  } else {
    glPtr->draw("CIErgb");
  }

  s->setUniformMatrix("modelMatrix", glm::mat4(1.f));
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
  projMatrix = glm::perspective(static_cast<float>(M_PI / 4.f), static_cast<float>(w) / static_cast<float>(h), 0.1f, 100.f);

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
    g_eye += glm::vec3(0, 0, -0.3f);
    g_center += glm::vec3(0, 0, -0.3f);
    cameraChanged = true;
    break;
  case 's':
    g_eye += glm::vec3(0, 0, 0.3f);
    g_center += glm::vec3(0, 0, 0.3f);
    cameraChanged = true;
    break;
  case 'a':
    g_eye += glm::vec3(-0.3f, 0, 0);
    g_center += glm::vec3(-0.3f, 0, 0);
    cameraChanged = true;
    break;
  case 'd':
    g_eye += glm::vec3(0.3f, 0, 0);
    g_center += glm::vec3(0.3f, 0, 0);
    cameraChanged = true;
    break;
  case 'r':
    g_eye += glm::vec3(0, 0.3f, 0);
    g_center += glm::vec3(0, 0.3f, 0);
    cameraChanged = true;
    break;
  case 'f':
    g_eye += glm::vec3(0, -0.3f, 0);
    g_center += glm::vec3(0, -0.3f, 0);
    cameraChanged = true;
    break;
  case ' ':
    g_wireRender = !g_wireRender;
    break;
  }

  if(g_wireRender) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

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
    g_xyzRender = !g_xyzRender;
    if(g_xyzRender)
      g_eye = glm::vec3(0.5, 1.4, 1.5);
    else
      g_eye = glm::vec3(1.0, 1.4, 2.8);
    cameraChanged = true;
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
