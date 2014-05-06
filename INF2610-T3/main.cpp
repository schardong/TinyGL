#include "tglconfig.h"
#include "tinygl.h"
#include "logger.h"
#include "shader.h"
#include "mesh.h"
#include "grid.h"
#include "sphere.h"
#include "quad.h"
#include "light.h"

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
static const int NUM_LIGHTS = 50;
static const int WINDOW_W = 800;
static const int WINDOW_H = 600;

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

bool initCalled = false;
bool initGLEWCalled = false;

GLuint lightTexBuff;
BufferObject* lightbuff;

enum {
  MATERIAL,
  NORMAL,
  VERTEX,
  num_buffers
};

GLuint g_fboId;
GLuint g_colorId[num_buffers];
GLuint g_depthId;

void setupLights();
void setupFBO(GLuint w, GLuint h);
void setupShaders();
void setupGeometry();

void drawSphere(size_t num_points)
{
  glDrawElements(GL_TRIANGLES, num_points, GL_UNSIGNED_INT, NULL);
}

void drawGrid(size_t num_points)
{
  glDrawElements(GL_TRIANGLES, num_points, GL_UNSIGNED_INT, NULL);
}

void drawQuad(size_t num_points)
{
  glDrawElements(GL_TRIANGLE_STRIP, num_points, GL_UNSIGNED_BYTE, NULL);
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

  g_window = glutCreateWindow("INF2610-T3");
  glutReshapeFunc(reshape);
  glutDisplayFunc(update);
  glutKeyboardFunc(keyPress);

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

  glClearColor(0.f, 0.f, 0.f, 0.f);
  glEnable(GL_DEPTH_TEST);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glPointSize(5);

  initGLEWCalled = true;
}

void init()
{
  g_eye = glm::vec3(0, 7, 15);
  g_center = glm::vec3(0, 0, 0);
  viewMatrix = glm::lookAt(g_eye, g_center, glm::vec3(0, 1, 0));
  projMatrix = glm::perspective(static_cast<float>(M_PI / 4.f), 1.f, 0.1f, 100.f);

  setupGeometry();
  setupShaders();
  setupFBO(WINDOW_W, WINDOW_H);
  setupLights();

  Shader* s = TinyGL::getInstance()->getShader("sPass");
  Mesh* quad = TinyGL::getInstance()->getMesh("screenQuad");
  s->setUniformMatrix("modelMatrix", quad->m_modelMatrix);
  s->setUniform4fv("u_materialColor", quad->getMaterialColor());

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, g_colorId[MATERIAL]);
  s->setUniform1i("u_diffuseMap", 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, g_colorId[NORMAL]);
  s->setUniform1i("u_normalMap", 1);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, g_colorId[VERTEX]);
  s->setUniform1i("u_vertexMap", 2);

  initCalled = true;
}

void destroy()
{
  TinyGL::getInstance()->freeResources();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, 0);
  
  glDeleteTextures(num_buffers, g_colorId);
  glDeleteRenderbuffers(1, &g_depthId);
  glDeleteFramebuffers(1, &g_fboId);
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

  //First pass. Filling the geometry buffers.
  glBindFramebuffer(GL_FRAMEBUFFER, g_fboId);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  GLfloat uiZeros[4] = {0.f, 0.f, 0.f, 0.f};
  GLfloat fOnes[4] = { 1.f, 1.f, 1.f, 1.f };
  glClearBufferfv(GL_COLOR, 0, uiZeros);
  glClearBufferfv(GL_COLOR, 1, uiZeros);
  glClearBufferfv(GL_COLOR, 2, uiZeros);
  glClearBufferfv(GL_DEPTH, 0, fOnes);

  TinyGL* glPtr = TinyGL::getInstance();
  Shader* s = glPtr->getShader("fPass");
  
  s->bind();
  for (int i = 0; i < NUM_SPHERES; i++) {
    s->setUniformMatrix("modelMatrix", TinyGL::getInstance()->getMesh("sphere" + to_string(i))->m_modelMatrix);
    s->setUniformMatrix("normalMatrix", TinyGL::getInstance()->getMesh("sphere" + to_string(i))->m_normalMatrix);
    s->setUniform4fv("u_materialColor", TinyGL::getInstance()->getMesh("sphere" + to_string(i))->getMaterialColor());
    glPtr->draw("sphere" + to_string(i));
  }

  /*for (int i = 0; i < NUM_LIGHTS; i++) {
    s->setUniformMatrix("modelMatrix", TinyGL::getInstance()->getMesh("lightMesh" + to_string(i))->m_modelMatrix);
    s->setUniform4fv("u_materialColor", TinyGL::getInstance()->getMesh("lightMesh" + to_string(i))->getMaterialColor());
    glPtr->draw("lightMesh" + to_string(i));
  }*/

  s->setUniformMatrix("modelMatrix", TinyGL::getInstance()->getMesh("ground")->m_modelMatrix);
  s->setUniformMatrix("normalMatrix", TinyGL::getInstance()->getMesh("ground")->m_normalMatrix);
  s->setUniform4fv("u_materialColor", TinyGL::getInstance()->getMesh("ground")->getMaterialColor());
  glPtr->draw("ground");
  
  glBindVertexArray(0);
  Shader::unbind();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //Second pass. Shading occurs here.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  s = glPtr->getShader("sPass");
  s->bind();
  //s->setUniformMatrix("viewMatrix", viewMatrix);
  glDisable(GL_DEPTH_TEST);

  glPtr->draw("screenQuad");
  
  glutSwapBuffers();
  glutPostRedisplay();
  glEnable(GL_DEPTH_TEST);
}

void reshape(int w, int h)
{
  if (!initCalled || !initGLEWCalled)
    return;

  glActiveTexture(GL_TEXTURE3);
  for (int i = 0; i < num_buffers; i++) {
    glBindTexture(GL_TEXTURE_2D, g_colorId[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, NULL);
  }

  glBindRenderbuffer(GL_RENDERBUFFER, g_depthId);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, g_depthId);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glViewport(0, 0, w, h);
  projMatrix = glm::perspective(static_cast<float>(M_PI / 4.f), static_cast<float>(w) / static_cast<float>(h), 0.1f, 100.f);

  Shader* s = TinyGL::getInstance()->getShader("fPass");
  s->bind();
  s->setUniformMatrix("projMatrix", projMatrix);

  Shader::unbind();
}

void keyPress(unsigned char c, int x, int y)
{
  bool cameraChanged = false;
  //printf("%d\n", c);
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
  }

  if (cameraChanged) {
    viewMatrix = glm::lookAt(g_eye, g_center, glm::vec3(0, 1, 0));

    Shader* s = TinyGL::getInstance()->getShader("fPass");
    s->bind();
    s->setUniformMatrix("viewMatrix", viewMatrix);
    
    s = TinyGL::getInstance()->getShader("sPass");
    s->bind();
    s->setUniformMatrix("viewMatrix", viewMatrix);

    /*BufferObject* ubuff = TinyGL::getInstance()->getBuffer("light_buff");
    ubuff->bind();
    GLfloat* buff = new GLfloat[4 * NUM_LIGHTS];
    
    for (int i = 0; i < NUM_LIGHTS; i++) {
      glm::vec4 tmp = glm::vec4(lightCoords[4 * i], lightCoords[4 * i + 1], lightCoords[4 * i + 2], lightCoords[4 * i + 3]);
      tmp = viewMatrix * tmp;
      buff[4 * i] = tmp.x;
      buff[4 * i + 1] = tmp.y;
      buff[4 * i + 2] = tmp.z;
      buff[4 * i + 3] = tmp.w;
    }

    ubuff->sendData(buff);
    delete buff;*/

    Shader::unbind();
  }
}

void exit_cb()
{
  glutDestroyWindow(g_window);
  destroy();
  exit(EXIT_SUCCESS);
}

void setupLights()
{
  //Sphere** lightMesh = new Sphere*[NUM_LIGHTS];
  Light** lightSources = new Light*[NUM_LIGHTS];

  for (int i = 0; i < NUM_LIGHTS; i++) {
    lightSources[i] = new Light();
    lightSources[i]->setPosition(glm::vec3(rand() % 50, 5 + rand() % 10, rand() % 50));
    lightSources[i]->setColor(glm::vec3((float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX));
    TinyGL::getInstance()->addResource(LIGHT, "light" + to_string(i), lightSources[i]);

    /*lightMesh[i] = new Sphere(20, 20);
    lightMesh[i]->setDrawCb(drawSphere);
    lightMesh[i]->setMaterialColor(glm::vec4(lightSources[i]->getColor(), 1.f));
    lightMesh[i]->m_modelMatrix = glm::translate(glm::vec3(lightSources[i]->getPosition())) * glm::scale(glm::vec3(0.1f));
    TinyGL::getInstance()->addResource(MESH, "lightMesh" + to_string(i), lightMesh[i]);*/
  }

  GLfloat* lightCoords = new GLfloat[4 * NUM_LIGHTS];
  for (int i = 0; i < NUM_LIGHTS; i++) {
    glm::vec3 pos = lightSources[i]->getPosition();

    lightCoords[i * 4] = pos.x;
    lightCoords[i * 4 + 1] = pos.y;
    lightCoords[i * 4 + 2] = pos.z;
    lightCoords[i * 4 + 3] = 1.f;
  }

  /*GLfloat* lightColors = new GLfloat[4 * NUM_LIGHTS];
  for (int i = 0; i < NUM_LIGHTS; i++) {
    glm::vec3 color = lightSources[i]->getColor();

    lightColors[i * 4] = color.x;
    lightColors[i * 4 + 1] = color.y;
    lightColors[i * 4 + 2] = color.z;
    lightColors[i * 4 + 3] = 1.f;
  }*/

  Shader* s = TinyGL::getInstance()->getShader("sPass");

  GLuint idxPos = glGetUniformBlockIndex(s->getProgramId(), "LightPos");
  glUniformBlockBinding(s->getProgramId(), idxPos, 0);
  /*GLuint idxColor = glGetUniformBlockIndex(s->getProgramId(), "LightColor");
  glUniformBlockBinding(s->getProgramId(), idxColor, 1);*/

  BufferObject* ubuffLightPos = new BufferObject(GL_UNIFORM_BUFFER, sizeof(GLfloat)* 3 * NUM_LIGHTS, GL_STATIC_DRAW);
  ubuffLightPos->sendData(lightCoords);

  /*BufferObject* ubuffLightColor = new BufferObject(GL_UNIFORM_BUFFER, sizeof(GLfloat)* 3 * NUM_LIGHTS, GL_STATIC_DRAW);
  ubuffLightColor->sendData(lightColors);*/

  glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubuffLightPos->getId());
  //glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubuffLightColor->getId());
  TinyGL::getInstance()->addResource(BUFFER, "lightpos_buff", ubuffLightPos);
  //TinyGL::getInstance()->addResource(BUFFER, "lightcolor_buff", ubuffLightColor);

  delete lightCoords;
  //delete lightColors;
}

void setupFBO(GLuint w, GLuint h)
{
  glGenFramebuffers(1, &g_fboId);
  glBindFramebuffer(GL_FRAMEBUFFER, g_fboId);

  glGenTextures(num_buffers, g_colorId);

  for (int i = 0; i < num_buffers; i++) {
    glBindTexture(GL_TEXTURE_2D, g_colorId[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, g_colorId[i], 0);
  }

  glGenRenderbuffers(1, &g_depthId);
  glBindRenderbuffer(GL_RENDERBUFFER, g_depthId);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, g_depthId);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  switch (fboStatus) {
  case GL_FRAMEBUFFER_UNDEFINED:
    Logger::getInstance()->error("FBO undefined");
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
    Logger::getInstance()->error("FBO incomplete attachment");
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
    Logger::getInstance()->error("FBO missing attachment");
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
    Logger::getInstance()->error("FBO incomplete draw buffer");
    break;
  case GL_FRAMEBUFFER_UNSUPPORTED:
    Logger::getInstance()->error("FBO unsupported");
    break;
  case GL_FRAMEBUFFER_COMPLETE:
    Logger::getInstance()->log("FBO created successfully");
    break;
  default:
    Logger::getInstance()->error("FBO undefined problem");
  }

  GLenum drawBuffer[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
  glDrawBuffers(num_buffers, drawBuffer);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void setupShaders()
{
  Shader* g_fPass = new Shader("../Resources/def_fpass.vs", "../Resources/def_fpass.fs");
  g_fPass->bind();
  g_fPass->setUniformMatrix("viewMatrix", viewMatrix);
  g_fPass->setUniformMatrix("projMatrix", projMatrix);

  Shader* g_sPass = new Shader("../Resources/def_spass.vs", "../Resources/def_spass.fs");
  g_sPass->bind();
  g_sPass->bindFragDataLoc("fColor", 0);
  g_sPass->setUniformMatrix("viewMatrix", viewMatrix);
  g_sPass->setUniformMatrix("projMatrix", glm::ortho(-1.f, 1.f, -1.f, 1.f));

  TinyGL::getInstance()->addResource(SHADER, "fPass", g_fPass);
  TinyGL::getInstance()->addResource(SHADER, "sPass", g_sPass);
}

void setupGeometry()
{
  Grid* ground;
  Sphere** spheres;
  Quad* screenQuad;

  ground = new Grid(10, 10);
  ground->setDrawCb(drawGrid);
  ground->setMaterialColor(glm::vec4(0.4, 0.6, 0.0, 1.0));
  ground->m_modelMatrix = glm::scale(glm::vec3(50, 1, 50)) * glm::rotate(static_cast<float>(M_PI / 2), glm::vec3(1, 0, 0));
  ground->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * ground->m_modelMatrix));
  TinyGL::getInstance()->addResource(MESH, "ground", ground);

  spheres = new Sphere*[NUM_SPHERES];
  for (int i = 0; i < NUM_SPHERES; i++) {
    spheres[i] = new Sphere(32, 32);
    spheres[i]->setDrawCb(drawSphere);
    spheres[i]->setMaterialColor(glm::vec4(1.0, 0.0, 0.0, 1.0));
  }

  for (int i = 0; i < W_SPHERES; i++) {
    for (int j = 0; j < H_SPHERES; j++) {
      spheres[i * W_SPHERES + j]->m_modelMatrix = glm::translate(glm::vec3(i * 5, 1.5, j * 5)) * glm::scale(glm::vec3(1.5));
      spheres[i * W_SPHERES + j]->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * spheres[i * W_SPHERES + j]->m_modelMatrix));
    }
  }

  for (int i = 0; i < NUM_SPHERES; i++) {
    TinyGL::getInstance()->addResource(MESH, "sphere" + to_string(i), spheres[i]);
  }

  screenQuad = new Quad();
  screenQuad->setDrawCb(drawQuad);
  screenQuad->setMaterialColor(glm::vec4(0.f, 0.f, 0.f, 1.f));
  screenQuad->m_modelMatrix = glm::mat4(1.f);
  screenQuad->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * screenQuad->m_modelMatrix));
  TinyGL::getInstance()->addResource(MESH, "screenQuad", screenQuad);
}