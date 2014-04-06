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
Grid* ground;
Axis* axis;

glm::mat4 viewMatrix;
glm::mat4 projMatrix;
glm::vec3 g_eye;
glm::vec3 g_center;
glm::vec3 g_light;

bool initCalled = false;
bool initGLEWCalled = false;

bool g_wireRender = false;

void drawGrid(size_t num_points)
{
  glDrawElements(GL_TRIANGLES, num_points, GL_UNSIGNED_INT, NULL);
}

void drawPoints(size_t num_points)
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
  glPointSize(3);

  initGLEWCalled = true;
}

void init()
{
  g_eye = glm::vec3(0, 2, 2);
  g_center = glm::vec3(0, 0, 0);
  g_light = glm::vec3(0, 6, 4);
  viewMatrix = glm::lookAt(g_eye, g_center, glm::vec3(0, 1, 0));
  projMatrix = glm::perspective(static_cast<float>(M_PI / 4.f), 1.f, 0.1f, 100.f);

  printf("lambda to CIExyz.\n");
  std::vector<glm::vec3> xyz;
  float xbar, ybar, zbar;

  for(float i = 0; i < 400; i += 1) {
    corGetCIExyz(380.f + i, &xbar, &ybar, &zbar);
    float x, y, z;

    if((xbar + ybar + zbar) == 0) {
      x = y = z = 0.f;
    } else {
      x = xbar / (xbar + ybar + zbar);
      y = ybar / (xbar + ybar + zbar);
      z = zbar / (xbar + ybar + zbar);
    }

    xyz.push_back(glm::vec3(x, y, z));

    //printf ("%d    %.5f  %.5f  %.5f\n", 380.f + i, x, y, z);
  }

  ciexyz = new CIExyzMesh(xyz);
  ciexyz->setDrawCb(drawLinesIdx);
  ciexyz->setMaterialColor(glm::vec4(0));
  TinyGL::getInstance()->addMesh("CIExyz", ciexyz);
  ciexyz->m_modelMatrix = glm::mat4(1.f);
  ciexyz->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * ciexyz->m_modelMatrix));

//  createMeshCIExyz(xbar, ybar, zbar, 400);

//  printf("CIExyz to CIErgb.\n");
//  float r[400], g[400], b[400];
//  for(int i = 0; i < 400; i++) {
//    corCIEXYZtoCIERGB(xbar[i], ybar[i], zbar[i], &r[i], &g[i], &b[i]);
//    printf ("%.5f\t%.5f  %.5f  %.5f\n", lambda[i], r[i], g[i], b[i]);
//  }

  axis = new Axis(glm::vec2(-1, 2), glm::vec2(-1, 2), glm::vec2(-1, 2));
  axis->setDrawCb(drawAxis);
  axis->setMaterialColor(glm::vec4(0.f));
  TinyGL::getInstance()->addMesh("axis", axis);
  axis->m_modelMatrix = glm::mat4(1.f);
  axis->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * axis->m_modelMatrix));

  ground = new Grid(10, 10);
  ground->setDrawCb(drawGrid);
  ground->setMaterialColor(glm::vec4(0.4, 0.6, 0.0, 1.0));
  TinyGL::getInstance()->addMesh("ground", ground);
  ground->m_modelMatrix = glm::scale(glm::vec3(20, 1, 20)) * glm::rotate(static_cast<float>(M_PI / 2), glm::vec3(1, 0, 0));
  ground->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * ground->m_modelMatrix));

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
//  for (int i = 0; i < NUM_SPHERES; i++) {
//    s->setUniformMatrix("modelMatrix", spheres[i]->m_modelMatrix);
//    s->setUniformMatrix("normalMatrix", spheres[i]->m_normalMatrix);
//    s->setUniform4fv("u_materialColor", spheres[i]->getMaterialColor());
//    glPtr->draw("sphere" + to_string(i));
//  }

//  s->setUniformMatrix("modelMatrix", ground->m_modelMatrix);
//  s->setUniformMatrix("normalMatrix", ground->m_normalMatrix);
//  s->setUniform4fv("u_materialColor", ground->getMaterialColor());
//  glPtr->draw("ground");

  s->setUniformMatrix("modelMatrix", glm::mat4(1.f));
  glPtr->draw("CIExyz");

  s->setUniformMatrix("modelMatrix", glm::mat4(1.f));
  glPtr->draw("axis");

//  s->setUniformMatrix("modelMatrix", light->m_modelMatrix);
//  s->setUniformMatrix("normalMatrix", light->m_normalMatrix);
//  s->setUniform4fv("u_materialColor", light->getMaterialColor());
//  glPtr->draw("light01");

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
  printf("%d\n", c);
  glm::vec3 back = g_eye - g_center;
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
  case 'i':
    //back = g_eye - g_center;
    back = glm::mat3(glm::rotate((float)M_PI / 100.f, glm::vec3(1, 0, 0))) * back;
    g_eye = back + g_center;
    cameraChanged = true;
    break;
  case 'k':
    //back = g_eye - g_center;
    back = glm::mat3(glm::rotate(-(float)M_PI / 100.f, glm::vec3(1, 0, 0))) * back;
    g_eye = back + g_center;
    cameraChanged = true;
    break;
  case 'j':
    //back = g_eye - g_center;
    back = glm::mat3(glm::rotate((float)M_PI / 100.f, glm::vec3(0, 1, 0))) * back;
    g_eye = back + g_center;
    cameraChanged = true;
    break;
  case 'l':
    //back = g_eye - g_center;
    back = glm::mat3(glm::rotate(-(float)M_PI / 100.f, glm::vec3(0, 1, 0))) * back;
    g_eye = back + g_center;
    cameraChanged = true;
    break;
//  case 'o':
//    g_points++;
//    cout << g_points << endl;
//    break;
//  case 'p':
//    g_points--;
//    cout << g_points << endl;
//    break;
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
  bool lightChanged = false;

  printf("%d\n", c);
  switch (c) {
//  case GLUT_KEY_LEFT:
//    g_light.x -= 0.1f;
//    light->m_modelMatrix = glm::translate(g_light);
//    light->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * light->m_modelMatrix));
//    lightChanged = true;
//    break;
//  case GLUT_KEY_RIGHT:
//    g_light.x += 0.1f;
//    light->m_modelMatrix = glm::translate(g_light);
//    light->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * light->m_modelMatrix));
//    lightChanged = true;
//    break;
//  case GLUT_KEY_UP:
//    g_light.z -= 0.1f;
//    light->m_modelMatrix = glm::translate(g_light);
//    light->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * light->m_modelMatrix));
//    lightChanged = true;
//    break;
//  case GLUT_KEY_DOWN:
//    g_light.z += 0.1f;
//    light->m_modelMatrix = glm::translate(g_light);
//    light->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * light->m_modelMatrix));
//    lightChanged = true;
//    break;
//  case GLUT_KEY_F1:
//    g_light.y += 0.1f;
//    light->m_modelMatrix = glm::translate(g_light);
//    light->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * light->m_modelMatrix));
//    lightChanged = true;
//    break;
//  case GLUT_KEY_F2:
//    g_light.y -= 0.1f;
//    light->m_modelMatrix = glm::translate(g_light);
//    light->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * light->m_modelMatrix));
//    lightChanged = true;
//    break;
//  case GLUT_KEY_F3:
//    perVertex = !perVertex;
//    break;
  }

//  if (lightChanged) {
//    float tmp[] = { g_light[0], g_light[1], g_light[2] };

//    Shader* s = TinyGL::getInstance()->getShader("ads_vertex");
//    s->bind();
//    s->setUniformfv("u_lightCoord", tmp, 3);

//    s = TinyGL::getInstance()->getShader("ads_frag");
//    s->bind();
//    s->setUniformfv("u_lightCoord", tmp, 3);
//  }
}

void exit_cb()
{
  glutDestroyWindow(g_window);
  destroy();
  exit(EXIT_SUCCESS);
}
