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

glm::mat4 viewMatrix;
glm::mat4 projMatrix;
glm::vec3 g_eye;
glm::vec3 g_center;
glm::vec3 g_light;

bool initCalled = false;
bool initGLEWCalled = false;
bool perVertex = true;

int g_points = 0;
bool g_showIndex = true;

void drawSphere(size_t num_points)
{
  glDrawArrays(GL_POINTS, 0, g_points);
  //if (g_showIndex)
    glDrawElements(GL_TRIANGLES, num_points, GL_UNSIGNED_INT, NULL);
}

void drawGrid(size_t num_points)
{
  glDrawElements(GL_TRIANGLES, num_points, GL_UNSIGNED_INT, NULL);
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

  g_window = glutCreateWindow("INF2610-T2");
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
  glPointSize(5);

  initGLEWCalled = true;
}

void init()
{
  g_eye = glm::vec3(0, 7, 15);
  g_center = glm::vec3(0, 0, 0);
  g_light = glm::vec3(0, 6, 4);
  viewMatrix = glm::lookAt(g_eye, g_center, glm::vec3(0, 1, 0));
  projMatrix = glm::perspective(static_cast<float>(M_PI / 4.f), 1.f, 0.1f, 100.f);

  Grid* ground;
  Sphere** spheres;
  Sphere* light;

  ground = new Grid(10, 10);
  ground->setDrawCb(drawGrid);
  ground->setMaterialColor(glm::vec4(0.4, 0.6, 0.0, 1.0));
  ground->m_modelMatrix = glm::scale(glm::vec3(20, 1, 20)) * glm::rotate(static_cast<float>(M_PI / 2), glm::vec3(1, 0, 0));
  ground->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * ground->m_modelMatrix));
  TinyGL::getInstance()->addMesh("ground", ground);

  light = new Sphere(32, 32);
  light->setDrawCb(drawSphere);
  light->setMaterialColor(glm::vec4(1.0, 1.0, 0.0, 1.0));
  light->m_modelMatrix = glm::translate(g_light);
  light->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * light->m_modelMatrix));
  TinyGL::getInstance()->addMesh("light01", light);

  spheres = new Sphere*[NUM_SPHERES];
  for (int i = 0; i < NUM_SPHERES; i++) {
    spheres[i] = new Sphere(32, 32);
    spheres[i]->setDrawCb(drawSphere);
    spheres[i]->setMaterialColor(glm::vec4(1.0, 0.0, 0.0, 1.0));
  }

  for (int i = 0; i < W_SPHERES; i++) {
    for (int j = 0; j < H_SPHERES; j++) {
      spheres[i * W_SPHERES + j]->m_modelMatrix = glm::translate(glm::vec3(i * 2, 0.5, j * 2)) * glm::scale(glm::vec3(0.5));
      spheres[i * W_SPHERES + j]->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * spheres[i * W_SPHERES + j]->m_modelMatrix));
    }
  }

  for (int i = 0; i < NUM_SPHERES; i++) {
    TinyGL::getInstance()->addMesh("sphere" + to_string(i), spheres[i]);
  }

  Shader* g_adsVertex = new Shader("../Resources/ads_vertex.vs", "../Resources/ads_vertex.fs");
  g_adsVertex->bind();
  g_adsVertex->bindFragDataLoc("out_vColor", 0);
  g_adsVertex->setUniformMatrix("viewMatrix", viewMatrix);
  g_adsVertex->setUniformMatrix("projMatrix", projMatrix);

  Shader* g_adsFrag = new Shader("../Resources/ads_frag.vs", "../Resources/ads_frag.fs");
  g_adsFrag->bind();
  g_adsFrag->bindFragDataLoc("out_vColor", 0);
  g_adsFrag->setUniformMatrix("viewMatrix", viewMatrix);
  g_adsFrag->setUniformMatrix("projMatrix", projMatrix);

  float tmp[] = { g_light[0], g_light[1], g_light[2] };
  g_adsVertex->bind();
  g_adsVertex->setUniformfv("u_lightCoord", tmp, 3);

  g_adsFrag->bind();
  g_adsFrag->setUniformfv("u_lightCoord", tmp, 3);
  
  TinyGL::getInstance()->addShader("ads_vertex", g_adsVertex);
  TinyGL::getInstance()->addShader("ads_frag", g_adsFrag);

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
  Shader* s;
  
  if (perVertex)
    s = glPtr->getShader("ads_vertex");
  else
    s = glPtr->getShader("ads_frag");
  
  s->bind();
  for (int i = 0; i < NUM_SPHERES; i++) {
    s->setUniformMatrix("modelMatrix", TinyGL::getInstance()->getMesh("sphere" + to_string(i))->m_modelMatrix);
    s->setUniformMatrix("normalMatrix", TinyGL::getInstance()->getMesh("sphere" + to_string(i))->m_normalMatrix);
    s->setUniform4fv("u_materialColor", TinyGL::getInstance()->getMesh("sphere" + to_string(i))->getMaterialColor());
    glPtr->draw("sphere" + to_string(i));
  }

  s->setUniformMatrix("modelMatrix", TinyGL::getInstance()->getMesh("ground")->m_modelMatrix);
  s->setUniformMatrix("normalMatrix", TinyGL::getInstance()->getMesh("ground")->m_normalMatrix);
  s->setUniform4fv("u_materialColor", TinyGL::getInstance()->getMesh("ground")->getMaterialColor());
  glPtr->draw("ground");

  s->setUniformMatrix("modelMatrix", TinyGL::getInstance()->getMesh("light01")->m_modelMatrix);
  s->setUniformMatrix("normalMatrix", TinyGL::getInstance()->getMesh("light01")->m_normalMatrix);
  s->setUniform4fv("u_materialColor", TinyGL::getInstance()->getMesh("light01")->getMaterialColor());
  glPtr->draw("light01");
  
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

  Shader* s = TinyGL::getInstance()->getShader("ads_vertex");
  s->bind();
  s->setUniformMatrix("projMatrix", projMatrix);

  s = TinyGL::getInstance()->getShader("ads_frag");
  s->bind();
  s->setUniformMatrix("projMatrix", projMatrix);

  Shader::unbind();
}

void keyPress(unsigned char c, int x, int y)
{
  bool cameraChanged = false;
  //printf("%d\n", c);
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
  case 'o':
    g_points++;
    cout << g_points << endl;
    break;
  case 'p':
    g_points--;
    cout << g_points << endl;
    break;
  case ' ':
    g_showIndex = !g_showIndex;
    break;
  }

  if (cameraChanged) {
    viewMatrix = glm::lookAt(g_eye, g_center, glm::vec3(0, 1, 0));
    float tmp[] = {g_eye[0], g_eye[1], g_eye[2]};

    Shader* s = TinyGL::getInstance()->getShader("ads_vertex");
    s->bind();
    s->setUniformMatrix("viewMatrix", viewMatrix);
    s->setUniformfv("u_eyeCoord", tmp, 3);
    
    s = TinyGL::getInstance()->getShader("ads_frag");
    s->bind();
    s->setUniformMatrix("viewMatrix", viewMatrix);
    s->setUniformfv("u_eyeCoord", tmp, 3);
  }
}

void specialKeyPress(int c, int x, int y)
{
  bool lightChanged = false;
  Mesh* light_ptr = TinyGL::getInstance()->getMesh("light01");
  
  switch (c) {
  case GLUT_KEY_LEFT:
    g_light.x -= 0.1f;
    light_ptr->m_modelMatrix = glm::translate(g_light);
    light_ptr->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * light_ptr->m_modelMatrix));
    lightChanged = true;
    break;
  case GLUT_KEY_RIGHT:
    g_light.x += 0.1f;
    light_ptr->m_modelMatrix = glm::translate(g_light);
    light_ptr->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * light_ptr->m_modelMatrix));
    lightChanged = true;
    break;
  case GLUT_KEY_UP:
    g_light.z -= 0.1f;
    light_ptr->m_modelMatrix = glm::translate(g_light);
    light_ptr->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * light_ptr->m_modelMatrix));
    lightChanged = true;
    break;
  case GLUT_KEY_DOWN:
    g_light.z += 0.1f;
    light_ptr->m_modelMatrix = glm::translate(g_light);
    light_ptr->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * light_ptr->m_modelMatrix));
    lightChanged = true;
    break;
  case GLUT_KEY_F1:
    g_light.y += 0.1f;
    light_ptr->m_modelMatrix = glm::translate(g_light);
    light_ptr->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * light_ptr->m_modelMatrix));
    lightChanged = true;
    break;
  case GLUT_KEY_F2:
    g_light.y -= 0.1f;
    light_ptr->m_modelMatrix = glm::translate(g_light);
    light_ptr->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * light_ptr->m_modelMatrix));
    lightChanged = true;
    break;
  case GLUT_KEY_F3:
    perVertex = !perVertex;
    break;
  }

  if (lightChanged) {
    float tmp[] = { g_light[0], g_light[1], g_light[2] };

    Shader* s = TinyGL::getInstance()->getShader("ads_vertex");
    s->bind();
    s->setUniformfv("u_lightCoord", tmp, 3);

    s = TinyGL::getInstance()->getShader("ads_frag");
    s->bind();
    s->setUniformfv("u_lightCoord", tmp, 3);
  }
}

void exit_cb()
{
  glutDestroyWindow(g_window);
  destroy();
  exit(EXIT_SUCCESS);
}
