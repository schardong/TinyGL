#include "config.h"
#include "tinygl.h"
#include "logger.h"
#include "shader.h"
#include "mesh.h"
#include "grid.h"
#include "sphere.h"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <string>
#include <vector>

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

static const int W_SPHERES = 10;
static const int H_SPHERES = 10;
static const int NUM_SPHERES = W_SPHERES * H_SPHERES;

using namespace std;
using namespace ggraf;

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

Shader* simple;

Grid* ground;
Sphere** spheres;

glm::mat4 viewMatrix;
glm::mat4 projMatrix;

bool initCalled = false;
bool initGLEWCalled = false;

void drawSphere()
{
  glDrawElements(GL_TRIANGLES, 15 * 15 * 6, GL_UNSIGNED_INT, NULL);
  //glDrawArrays(GL_POINTS, 0, num_points);
}

void drawGrid()
{
  glDrawElements(GL_TRIANGLES, 9 * 9 * 6, GL_UNSIGNED_INT, NULL);
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

  g_window = glutCreateWindow("INF2610-T1");
  glutReshapeFunc(reshape);
  glutDisplayFunc(update);
  glutKeyboardFunc(keyPress);
  glutSpecialFunc(specialKeyPress);

  glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
  glutCloseFunc(exit_cb);
}

void initGLEW()
{
  GLenum err = glewInit();

  if (err != GLEW_OK) {
    Logger::getInstance()->error("Failed to initialize GLEW");
    std::cerr << glewGetErrorString(err) << std::endl;
    exit(1);
  }

  glClearColor(1.f, 1.f, 1.f, 1.f);
  glEnable(GL_DEPTH_TEST);

  initGLEWCalled = true;
}

void init()
{
  ground = new Grid(10, 10);
  ground->setDrawCb(drawGrid);

  spheres = new Sphere*[NUM_SPHERES];

  for (int i = 0; i < NUM_SPHERES; i++) {
    spheres[i] = new Sphere(15, 15);
    spheres[i]->setDrawCb(drawSphere);
    TinyGL::getInstance()->addMesh("sphere" + to_string(i), spheres[i]);
  }

  for (int i = 0; i < W_SPHERES; i++) {
    for (int j = 0; j < H_SPHERES; j++) {
      spheres[i * W_SPHERES + j]->m_modelMatrix = glm::translate(glm::vec3(i, 0.4, j)) * glm::scale(glm::vec3(0.4, 0.4, 0.4));
    }
  }

  viewMatrix = glm::lookAt(glm::vec3(0, 7, 15), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  projMatrix = glm::perspective(static_cast<float>(M_PI / 3.f), 1.f, 1.f, 100.f);

  simple = new Shader("../Resources/simple.vs", "../Resources/simple.fs", "../Resources/simple.gs");
  simple->bind();
  simple->bindFragDataLoc("out_vColor", 0);
  simple->setUniformMatrix("viewMatrix", viewMatrix);
  simple->setUniformMatrix("projMatrix", projMatrix);

  /*points = new Shader("../Resources/simple.vs", "../Resources/simple.fs", "../Resources/points.gs");
  points->bind();
  points->bindFragDataLoc("out_fColor", 0);
  points->setUniformMatrix("viewMatrix", viewMatrix);
  points->setUniformMatrix("projMatrix", projMatrix);*/

  TinyGL::getInstance()->addMesh("ground", ground);
  TinyGL::getInstance()->addShader("simple", simple);
  //TinyGL::getInstance()->addShader("points", points);

  ground->m_modelMatrix = glm::scale(glm::vec3(10, 1, 10)) /* glm::translate(glm::vec3(-10, 0, -10)) */* glm::rotate(static_cast<float>(M_PI / 2), glm::vec3(1, 0, 0));
  ground->m_normalMatrix = glm::inverseTranspose(viewMatrix * ground->m_modelMatrix);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glPointSize(3.f);

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
  ggraf::Shader* s = glPtr->getShader("simple");
  ggraf::Shader* p = glPtr->getShader("points");
  
  s->bind();
  for (int i = 0; i < NUM_SPHERES; i++) {
    s->setUniformMatrix("modelMatrix", spheres[i]->m_modelMatrix);
    s->setUniformMatrix("normalMatrix", spheres[i]->m_normalMatrix);
    glPtr->draw("sphere" + to_string(i));
  }

  s->setUniformMatrix("modelMatrix", ground->m_modelMatrix);
  s->setUniformMatrix("normalMatrix", ground->m_normalMatrix);
  glPtr->draw("ground");

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
  projMatrix = glm::perspective(static_cast<float>(M_PI / 3.f), static_cast<float>(w) / static_cast<float>(h), 1.f, 100.f);

  Shader* s = TinyGL::getInstance()->getShader("simple");

  s->bind();
  s->setUniformMatrix("projMatrix", projMatrix);
  s->setUniformMatrix("viewMatrix", viewMatrix);

  Shader::unbind();
}

void keyPress(unsigned char c, int x, int y)
{
  bool cameraChanged = false;
  printf("%d\n", c);
  switch (c) {
  case 'w':
    viewMatrix *= glm::translate(glm::vec3(0, 0, 1));
    cameraChanged = true;
    break;
  case 's':
    viewMatrix *= glm::translate(glm::vec3(0, 0, -1));
    cameraChanged = true;
    break;
  case 'a':
    viewMatrix *= glm::translate(glm::vec3(1, 0, 0));
    cameraChanged = true;
    break;
  case 'd':
    viewMatrix *= glm::translate(glm::vec3(-1, 0, 0));
    cameraChanged = true;
    break;
  case 'r':
    viewMatrix *= glm::translate(glm::vec3(0, -1, 0));
    cameraChanged = true;
    break;
  case 'f':
    viewMatrix *= glm::translate(glm::vec3(0, 1, 0));
    cameraChanged = true;
    break;
  case 'm':
    viewMatrix *= glm::rotate((float)-M_PI / 25, glm::vec3(0, 0, 1));
    cameraChanged = true;
    break;
  case 'n':
    viewMatrix *= glm::rotate((float)M_PI / 25, glm::vec3(0, 0, 1));
    cameraChanged = true;
    break;

  }

  if (cameraChanged) {
    Shader* s = TinyGL::getInstance()->getShader("simple");
    Shader* p = TinyGL::getInstance()->getShader("points");

    s->bind();
    s->setUniformMatrix("viewMatrix", viewMatrix);

    p->bind();
    p->setUniformMatrix("viewMatrix", viewMatrix);
  }
}

void specialKeyPress(int c, int x, int y)
{
  bool cameraChanged = false;

  switch (c) {
  case GLUT_KEY_LEFT:
    viewMatrix *= glm::rotate((float)M_PI / 25.f, glm::vec3(0, 1, 0));
    cameraChanged = true;
    break;
  case GLUT_KEY_RIGHT:
    viewMatrix *= glm::rotate((float)-M_PI / 25, glm::vec3(0, 1, 0));
    cameraChanged = true;
    break;
  case GLUT_KEY_UP:
    viewMatrix *= glm::rotate((float)M_PI / 25, glm::vec3(1, 0, 0));
    cameraChanged = true;
    break;
  case GLUT_KEY_DOWN:
    viewMatrix *= glm::rotate((float)-M_PI / 25, glm::vec3(1, 0, 0));
    cameraChanged = true;
    break;
  }

  if (cameraChanged) {
    Shader* s = TinyGL::getInstance()->getShader("simple");
    Shader* p = TinyGL::getInstance()->getShader("points");

    s->bind();
    s->setUniformMatrix("viewMatrix", viewMatrix);

    p->bind();
    p->setUniformMatrix("viewMatrix", viewMatrix);
  }
}

void exit_cb()
{
  glutDestroyWindow(g_window);
  destroy();
  exit(EXIT_SUCCESS);
}