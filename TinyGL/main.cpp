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

Shader* s1;

Mesh* p1;
Mesh* p2;
Mesh* p3;
Mesh* axes;

glm::mat4 viewMatrix;
glm::mat4 projMatrix;

Ihandle* canvas;
Ihandle* dialogue;

bool axisOn = false;
bool sphereOn = false;
bool gridOn = false;

bool initCalled = false;
bool initGLEWCalled = false;

void toggleAxis()
{
  axisOn = !axisOn;
}

void toggleSphere()
{
  sphereOn = !sphereOn;
}

void toggleGrid()
{
  gridOn = !gridOn;
}

void drawAxes()
{
  axes->bind();
  glDrawElements(GL_LINES, 6, GL_UNSIGNED_BYTE, NULL);
}

void drawSphere()
{
  p1->bind();
  //glDrawArrays(GL_LINE_STRIP, 0, 7350);
  glDrawElements(GL_TRIANGLES, 49 * 49 * 6, GL_UNSIGNED_INT, NULL);
}

void drawGrid()
{
  p3->bind();
  glDrawElements(GL_TRIANGLES, 4 * 4 * 6, GL_UNSIGNED_INT, NULL);
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
  Ihandle *finale, *axisToggle, *sphereToggle, *gridToggle, *toggleButtons;
  IupOpen(&argc, &argv);
  IupGLCanvasOpen();

  canvas = IupGLCanvas("draw");
    
  IupSetFunction("draw", (Icallback)draw);
  IupSetFunction(IUP_IDLE_ACTION, (Icallback)update);

  IupSetCallback(canvas, "RESIZE_CB", (Icallback)reshape);
  IupSetCallback(canvas, "K_ANY", (Icallback)keyPress);

  IupSetAttribute(canvas, IUP_BUFFER, IUP_DOUBLE);
  IupSetAttribute(canvas, "RASTERSIZE", "600x600");

  axisToggle = IupToggle("Toggle axes", NULL);
  IupSetCallback(axisToggle, "ACTION", (Icallback)toggleAxis);

  sphereToggle = IupToggle("Toggle sphere", NULL);
  IupSetCallback(sphereToggle, "ACTION", (Icallback)toggleSphere);

  gridToggle = IupToggle("Toggle grid", NULL);
  IupSetCallback(gridToggle, "ACTION", (Icallback)toggleGrid);

  toggleButtons = IupHbox(IupFill(), axisToggle, sphereToggle, gridToggle, IupFill(), NULL);
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

  p1 = new Mesh();
  p1->createSphereBuffer(50, 50);
  p1->setDrawCb(&drawSphere);
  p2 = new Mesh();
  //p2->createGridBuffer(30, 30);
  p3 = new Mesh();
  p3->createGridBuffer(5, 5);
  p3->setDrawCb(&drawGrid);
  axes = new Mesh();
  axes->createAxesBuffer();
  axes->setDrawCb(&drawAxes);

  viewMatrix = glm::lookAt(glm::vec3(3, 5, 7.5), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
  projMatrix = glm::perspective(static_cast<float>(M_PI / 4.f), 1.f, 1.f, 100.f);

  s1 = new Shader("simple.vs", "simple.fs");
  s1->bind();
  s1->bindFragDataLoc("out_vColor", 0);
  s1->setUniformMatrix("viewMatrix", viewMatrix);
  s1->setUniformMatrix("projMatrix", projMatrix);
  
  TinyGL::getInstance()->addMesh("sphere", p1);
  TinyGL::getInstance()->addMesh("grid", p3);
  TinyGL::getInstance()->addMesh("axes", axes);

  TinyGL::getInstance()->addShader("simple", s1);
  
  p1->m_modelMatrix = glm::translate(glm::vec3(0, 1, 0));
  p1->m_normalMatrix = glm::inverseTranspose(p1->m_modelMatrix);

  p2->m_modelMatrix = glm::translate(glm::vec3(0, 0, 0.2)) *
                      glm::scale(glm::vec3(15, 1, 15)) *
                      glm::rotate(static_cast<float>(M_PI / 2), glm::vec3(1, 0, 0));

  p3->m_modelMatrix = glm::scale(glm::vec3(10, 1, 10))*glm::rotate(static_cast<float>(M_PI / 2), glm::vec3(1, 0, 0));
  p3->m_normalMatrix = glm::inverseTranspose(p3->m_modelMatrix);

  axes->m_modelMatrix = glm::scale(glm::vec3(5.0, 5.0, 5.0));
  axes->m_normalMatrix = glm::inverseTranspose(axes->m_modelMatrix);

  initCalled = true;
}

void destroy()
{
  Shader::unbind();
  TinyGL::getInstance()->freeResources();
  delete s1;

  delete p1;
  delete p2;
  delete p3;
  delete axes;
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
  s->bind();

  if (axisOn) {
    s->setUniformMatrix("modelMatrix", axes->m_modelMatrix);
    s->setUniformMatrix("normalMatrix", axes->m_normalMatrix);
    glPtr->draw("axes");
  }

  if (sphereOn) {
    s->setUniformMatrix("modelMatrix", p1->m_modelMatrix);
    s->setUniformMatrix("normalMatrix", p1->m_normalMatrix);
    glPtr->draw("sphere");
  }

  if (gridOn) {
    s->setUniformMatrix("modelMatrix", p3->m_modelMatrix);
    s->setUniformMatrix("normalMatrix", p3->m_normalMatrix);
    glPtr->draw("grid");
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
  s1->bind();
  s1->setUniformMatrix("projMatrix", projMatrix);
  s1->setUniformMatrix("viewMatrix", viewMatrix);
  Shader::unbind();
  draw(canvas);
  return IUP_DEFAULT;
}

int keyPress(Ihandle* self, int c)
{
  if (c == K_q) {
    destroy();
    return IUP_CLOSE;
  } 
  return IUP_DEFAULT;
}