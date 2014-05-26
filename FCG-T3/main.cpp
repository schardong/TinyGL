#include "tglconfig.h"
#include "fcgt3config.h"
#include "tinygl.h"
#include "logger.h"
#include "shader.h"
#include "harris.h"
#include "quad.h"

extern "C" {
#include "image.h"
}

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <string>
#include <vector>

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <opencv2/calib3d/calib3d.hpp>

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

GLuint pattern_tex;

bool initCalled = false;
bool initGLEWCalled = false;

void createQuad();
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
  glutInitWindowSize(800, 600);

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

  Image* pattern_rgb = imgReadBMP("../Resources/padrao.bmp");
  Image* pattern = imgGrey(pattern_rgb);
  imgDestroy(pattern_rgb);
  Image* corners = imgCreate(imgGetWidth(pattern), imgGetHeight(pattern), 1);
  std::vector<glm::vec2> c = HarrisCornerDetector(pattern, corners);

  float* pattern_data = imgGetData(corners);

  Logger* log = Logger::getInstance();
  log->log("Found " + to_string(c.size()) + " corners in the given image.");
  for(auto it = c.begin(); it != c.end(); it++) {
    log->log("corner found at (" + to_string(it->x) +", " + to_string(it->y) + ")");
  }
  
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &pattern_tex);
  glBindTexture(GL_TEXTURE_2D, pattern_tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, imgGetWidth(pattern), imgGetHeight(pattern), 0, GL_RED, GL_FLOAT, pattern_data);
  glBindTexture(GL_TEXTURE_2D, 0);

  imgDestroy(corners);
  
  Shader* g_shader = new Shader("../Resources/fcgt2.vs", "../Resources/fcgt2.fs");
  g_shader->bind();
  g_shader->bindFragDataLoc("fColor", 0);
  g_shader->setUniform1i("u_image", 0);
  TinyGL::getInstance()->addResource(SHADER, "fcgt2", g_shader);

  initCalled = true;
  glutReshapeWindow(imgGetWidth(pattern), imgGetHeight(pattern));
}

void destroy()
{
  TinyGL::getInstance()->freeResources();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDeleteTextures(1, &pattern_tex);
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
  glBindTexture(GL_TEXTURE_2D, pattern_tex);

  s->setUniformMatrix("modelMatrix", glPtr->getMesh("quad")->m_modelMatrix);
  glPtr->getMesh("quad")->draw();

  glBindTexture(GL_TEXTURE_2D, pattern_tex);
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
}

void keyPress(unsigned char c, int x, int y)
{
  switch (c) {
  default:
    printf("(%d, %d) = %d, %c", x, y, c, c);
    break;
  }
}

void specialKeyPress(int c, int x, int y)
{
  switch (c) {
  default:
    printf("(%d, %d) = %d", x, y, c);
    break;
  }
}

void exit_cb()
{
  glutDestroyWindow(g_window);
  destroy();
  exit(EXIT_SUCCESS);
}

void drawQuad(size_t num_points)
{
  glDrawElements(GL_TRIANGLE_STRIP, num_points, GL_UNSIGNED_BYTE, NULL);
}