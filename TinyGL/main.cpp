#include "config.h"
#include "tinygl.h"
#include "logger.h"
#include "shader.h"
#include "mesh.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include <iup.h>
#include <iupgl.h>
#include <iostream>
#include <string>
#include <vector>

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

static const int W_SPHERES = 1;
static const int H_SPHERES = 1;
static const int NUM_SPHERES = W_SPHERES * H_SPHERES;

using namespace std;
using namespace ggraf;

void init();
void initIUP(int argc, char** argv);
void initGLEW();
void destroy();
int update(Ihandle* self);
int draw(Ihandle* self);
int reshape(Ihandle* self, int w, int h);
int keyPress(Ihandle* self, int c);
int exit_cb(Ihandle* self);

Shader* simple;
Shader* points;

Mesh* ground;
Mesh** spheres;

glm::mat4 viewMatrix;
glm::mat4 projMatrix;

Ihandle* canvas;
Ihandle* dialogue;

bool axisOn = false;
bool sphereOn = false;
bool gridOn = false;

bool initCalled = false;
bool initGLEWCalled = false;

void toggleSphere()
{
  sphereOn = !sphereOn;
}

void toggleGrid()
{
  gridOn = !gridOn;
}

void drawSphere()
{
  //glDrawElements(GL_TRIANGLES, 5 * 5 * 6, GL_UNSIGNED_INT, NULL);
  glDrawArrays(GL_POINTS, 0, 5 * 5 + 2);
}

void drawGrid()
{
  glDrawElements(GL_TRIANGLES, 9 * 9 * 6, GL_UNSIGNED_INT, NULL);
}

int main(int argc, char** argv)
{
  Logger::getInstance()->setLogStream(&cout);
  Logger::getInstance()->log(TINYGL_LIBNAME + string(" v") + to_string(TINYGL_MAJOR_VERSION) + "." + to_string(TINYGL_MINOR_VERSION));

  initIUP(argc, argv);
  initGLEW();
  init();

  IupMainLoop();
  IupClose();
  
	return 0;
}

void initIUP(int argc, char** argv)
{
  Ihandle *finale, *sphereToggle, *gridToggle, *toggleButtons;
  IupOpen(&argc, &argv);
  IupGLCanvasOpen();

  canvas = IupGLCanvas("draw");
    
  IupSetFunction("draw", (Icallback)draw);
  IupSetFunction(IUP_IDLE_ACTION, (Icallback)update);

  IupSetCallback(canvas, "RESIZE_CB", (Icallback)reshape);
  IupSetCallback(canvas, "K_ANY", (Icallback)keyPress);

  IupSetAttribute(canvas, IUP_BUFFER, IUP_DOUBLE);
  IupSetAttribute(canvas, "RASTERSIZE", "600x600");
  
  sphereToggle = IupToggle("Toggle sphere", NULL);
  IupSetCallback(sphereToggle, "ACTION", (Icallback)toggleSphere);

  gridToggle = IupToggle("Toggle grid", NULL);
  IupSetCallback(gridToggle, "ACTION", (Icallback)toggleGrid);

  toggleButtons = IupHbox(IupFill(), sphereToggle, gridToggle, IupFill(), NULL);
  finale = IupVbox(IupFill(), canvas, toggleButtons, IupFill(), NULL);

  dialogue = IupDialog(finale);
  IupSetAttribute(dialogue, "TITLE", TINYGL_LIBNAME);

  IupShow(dialogue);
  IupMap(canvas);
}

void initGLEW()
{
  IupGLMakeCurrent(canvas);
  GLenum err = glewInit();

  if (err != GLEW_OK) {
    Logger::getInstance()->error("Failed to initialize GLEW");
    std::cerr << glewGetErrorString(err) << std::endl;
    exit(1);
  }

  glClearColor(0.f, 0.f, 0.f, 1.f);
  glEnable(GL_DEPTH_TEST);

  initGLEWCalled = true;
}

void init()
{
  IupGLMakeCurrent(canvas);
  
  ground = createGridMesh(10, 10);
  ground->setDrawCb(drawGrid);

  spheres = new Mesh*[NUM_SPHERES];
  
  for (int i = 0; i < NUM_SPHERES; i++) {
    spheres[i] = createSphereMesh(5, 5);
    spheres[i]->setDrawCb(drawSphere);
    TinyGL::getInstance()->addMesh("sphere" + to_string(i), spheres[i]);
  }

  for (int i = 0; i < W_SPHERES; i++) {
    for (int j = 0; j < H_SPHERES; j++) {
      spheres[i * W_SPHERES + j]->m_modelMatrix = glm::translate(glm::vec3(3*i, 1, 3*j));
    }
  }

  viewMatrix = glm::lookAt(glm::vec3(0, 15, 0), glm::vec3(0, -1, 0), glm::vec3(0, 0, -1));
  projMatrix = glm::perspective(static_cast<float>(M_PI / 4.f), 1.f, 1.f, 100.f);

  simple = new Shader("simple.vs", "simple.fs", "simple.gs");
  simple->bind();
  simple->bindFragDataLoc("out_vColor", 0);
  simple->setUniformMatrix("viewMatrix", viewMatrix);
  simple->setUniformMatrix("projMatrix", projMatrix);

  points = new Shader("simple.vs", "simple.fs", "points.gs");
  points->bind();
  points->bindFragDataLoc("out_fColor", 0);
  points->setUniformMatrix("viewMatrix", viewMatrix);
  points->setUniformMatrix("projMatrix", projMatrix);
  
  TinyGL::getInstance()->addMesh("ground", ground);
  TinyGL::getInstance()->addShader("simple", simple);
  TinyGL::getInstance()->addShader("points", points);
    
  ground->m_modelMatrix = glm::scale(glm::vec3(30, 1, 30)) * glm::rotate(static_cast<float>(M_PI / 2), glm::vec3(1, 0, 0));
  ground->m_normalMatrix = glm::inverseTranspose(viewMatrix * ground->m_modelMatrix);
  
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glPointSize(3.f);

  initCalled = true;
}

void destroy()
{
  TinyGL::getInstance()->freeResources();
}

int update(Ihandle* self)
{
  if (!initCalled || !initGLEWCalled)
    return IUP_DEFAULT;

  draw(canvas);
  return IUP_DEFAULT;
}

int draw(Ihandle* self)
{
  if (!initCalled || !initGLEWCalled)
    return IUP_DEFAULT;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  TinyGL* glPtr = TinyGL::getInstance();
  ggraf::Shader* s = glPtr->getShader("simple");
  ggraf::Shader* p = glPtr->getShader("points");
  
  p->bind();
  if (sphereOn) {
    for (int i = 0; i < NUM_SPHERES; i++) {
      p->setUniformMatrix("modelMatrix", spheres[i]->m_modelMatrix);
      p->setUniformMatrix("normalMatrix", spheres[i]->m_normalMatrix);
      glPtr->draw("sphere" + to_string(i));
    }
  }

  s->bind();
  if (gridOn) {
    s->setUniformMatrix("modelMatrix", ground->m_modelMatrix);
    s->setUniformMatrix("normalMatrix", ground->m_normalMatrix);
    glPtr->draw("ground");
  }

  glBindVertexArray(0);
  Shader::unbind();

  IupGLSwapBuffers(self);
  return IUP_DEFAULT;
}

int reshape(Ihandle* self, int w, int h)
{
  if (!initCalled || !initGLEWCalled)
    return IUP_DEFAULT;

  IupGLMakeCurrent(self);
  glViewport(0, 0, w, h);
  projMatrix = glm::perspective(static_cast<float>(M_PI / 3.f), static_cast<float>(w) / static_cast<float>(h), 1.f, 100.f);
  
  Shader* s = TinyGL::getInstance()->getShader("simple");
  Shader* p = TinyGL::getInstance()->getShader("points");

  s->bind();
  s->setUniformMatrix("projMatrix", projMatrix);
  s->setUniformMatrix("viewMatrix", viewMatrix);

  p->bind();
  p->setUniformMatrix("projMatrix", projMatrix);
  p->setUniformMatrix("viewMatrix", viewMatrix);

  Shader::unbind();
  draw(canvas);
  return IUP_DEFAULT;
}

int keyPress(Ihandle* self, int c)
{
  bool cameraChanged = false;
  switch (c) {
  case K_q:
    destroy();
    return IUP_CLOSE;
    break;
  case K_w:
    viewMatrix *= glm::translate(glm::vec3(0, 0, 1));
    cameraChanged = true;
    break;
  case K_s:
    viewMatrix *= glm::translate(glm::vec3(0, 0, -1));
    cameraChanged = true;
    break;
  case K_a:
    viewMatrix *= glm::translate(glm::vec3(1, 0, 0));
    cameraChanged = true;
    break;
  case K_d:
    viewMatrix *= glm::translate(glm::vec3(-1, 0, 0));
    cameraChanged = true;
    break;
  case K_r:
    viewMatrix *= glm::translate(glm::vec3(0, -1, 0));
    cameraChanged = true;
    break;
  case K_f:
    viewMatrix *= glm::translate(glm::vec3(0, 1, 0));
    cameraChanged = true;
    break;
  case K_LEFT:
    viewMatrix *= glm::rotate(M_PI / 25.f, glm::vec3(0, 1, 0));
    cameraChanged = true;
    break;
  case K_RIGHT:
    viewMatrix *= glm::rotate(-M_PI / 25, glm::vec3(0, 1, 0));
    cameraChanged = true;
    break;
  case K_UP:
    viewMatrix *= glm::rotate(M_PI / 25, glm::vec3(1, 0, 0));
    cameraChanged = true;
    break;
  case K_DOWN:
    viewMatrix *= glm::rotate(-M_PI / 25, glm::vec3(1, 0, 0));
    cameraChanged = true;
    break;
  case K_m:
    viewMatrix *= glm::rotate(-M_PI / 25, glm::vec3(0, 0, 1));
    cameraChanged = true;
    break;
  case K_n:
    viewMatrix *= glm::rotate(M_PI / 25, glm::vec3(0, 0, 1));
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
    /*for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        cerr << viewMatrix[i][j] << " ";
      }
      cerr << endl;
    }
    cerr << endl;*/
  }

  return IUP_DEFAULT;
}