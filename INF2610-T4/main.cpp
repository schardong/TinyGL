#include "tglconfig.h"
#include "tinygl.h"
#include "logger.h"
#include "shader.h"
#include "mesh.h"
#include "cube.h"
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
static const int NUM_LIGHTS = 1;
static const int WINDOW_W = 1000;
static const int WINDOW_H = 1000;

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
float* g_depth;

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

void drawByteIdx(size_t num_points)
{
  glDrawElements(GL_TRIANGLES, num_points, GL_UNSIGNED_BYTE, NULL);
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
  glutInitWindowSize(WINDOW_W, WINDOW_H);

  g_window = glutCreateWindow("INF2610-T4");
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
  glDepthFunc(GL_LESS);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glClearDepth(1.0f);
  //glPointSize(5);

  initGLEWCalled = true;
}

void init()
{
  g_eye = glm::vec3(25, 7, 45);
  g_center = glm::vec3(25, 0, 0);
  viewMatrix = glm::lookAt(g_eye, g_center, glm::vec3(0, 1, 0));
  projMatrix = glm::perspective(static_cast<float>(M_PI / 4.f), 1.f, 1.f, 100.f);

  setupGeometry();
  setupShaders();
  setupFBO(WINDOW_W, WINDOW_H);
  setupLights();

  Shader* s = TinyGL::getInstance()->getShader("sPass");
  Mesh* quad = TinyGL::getInstance()->getMesh("screenQuad");
  s->setUniformMatrix("modelMatrix", quad->m_modelMatrix);
  s->setUniform4fv("u_materialColor", quad->getMaterialColor());
  s->setUniform1f("u_zNear", 1.f);
  s->setUniform1f("u_zFar", 100.f);

  float ss[2] = {WINDOW_W, WINDOW_H};
  s->setUniformfv("u_screenSize", ss, 2);

  g_depth = new float[WINDOW_W * WINDOW_H];

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, g_colorId[MATERIAL]);
  s->setUniform1i("u_diffuseMap", 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, g_colorId[NORMAL]);
  s->setUniform1i("u_normalMap", 1);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, g_colorId[VERTEX]);
  s->setUniform1i("u_vertexMap", 2);

  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, g_depthId);
  s->setUniform1i("u_depthMap", 4);

  initCalled = true;
}

void destroy()
{
  TinyGL::getInstance()->freeResources();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, 0);
  
  glDeleteTextures(num_buffers, g_colorId);
  glDeleteTextures(1, &g_depthId);
  glDeleteFramebuffers(1, &g_fboId);
  delete[] g_depth;
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
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;

  TinyGL* glPtr = TinyGL::getInstance();
  Shader* s = glPtr->getShader("fPass");
  
  s->bind();
  for (int i = 0; i < NUM_SPHERES; i++) {
    s->setUniformMatrix("modelMatrix", TinyGL::getInstance()->getMesh("sphere" + to_string(i))->m_modelMatrix);
    s->setUniformMatrix("normalMatrix", TinyGL::getInstance()->getMesh("sphere" + to_string(i))->m_normalMatrix);
    s->setUniform4fv("u_materialColor", TinyGL::getInstance()->getMesh("sphere" + to_string(i))->getMaterialColor());
    glPtr->draw("sphere" + to_string(i));
  }

  for(int i = 0; i < 5; i++) {
    s->setUniformMatrix("modelMatrix", TinyGL::getInstance()->getMesh("bottom_box" + to_string(i))->m_modelMatrix);
    s->setUniformMatrix("normalMatrix", TinyGL::getInstance()->getMesh("bottom_box" + to_string(i))->m_normalMatrix);
    s->setUniform4fv("u_materialColor", TinyGL::getInstance()->getMesh("bottom_box" + to_string(i))->getMaterialColor());
    glPtr->draw("bottom_box" + to_string(i));
  }
  
  /*s->setUniformMatrix("modelMatrix", TinyGL::getInstance()->getMesh("box1")->m_modelMatrix);
  s->setUniformMatrix("normalMatrix", TinyGL::getInstance()->getMesh("box1")->m_normalMatrix);
  s->setUniform4fv("u_materialColor", TinyGL::getInstance()->getMesh("box1")->getMaterialColor());
  glPtr->draw("box1");*/

  glBindVertexArray(0);
  Shader::unbind();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //Second pass. Shading occurs here.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  s = glPtr->getShader("sPass");
  s->bind();
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

  for (int i = 0; i < num_buffers; i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, g_colorId[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, NULL);
  }

  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, g_depthId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  glViewport(0, 0, w, h);
  projMatrix = glm::perspective(static_cast<float>(M_PI / 4.f), static_cast<float>(w) / static_cast<float>(h), 1.f, 100.f);

  Shader* s = TinyGL::getInstance()->getShader("fPass");
  s->bind();
  s->setUniformMatrix("projMatrix", projMatrix);
  float ss[2] = {w, h};
  s->setUniformfv("u_screenSize", ss, 2);

  s = TinyGL::getInstance()->getShader("sPass");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, g_colorId[MATERIAL]);
  s->setUniform1i("u_diffuseMap", 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, g_colorId[NORMAL]);
  s->setUniform1i("u_normalMap", 1);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, g_colorId[VERTEX]);
  s->setUniform1i("u_vertexMap", 2);

  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, g_depthId);
  s->setUniform1i("u_depthMap", 4);

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
    lightSources[i]->setPosition(glm::vec3(rand() % 35, 5 + rand() % 10, rand() % 35));
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

  for (int i = 0; i < NUM_LIGHTS; i++)
    cout << "(" << lightCoords[i * 4] << ", " << lightCoords[i * 4 + 1] << ", " << lightCoords[i * 4 + 2] << ", " << lightCoords[i * 4 + 3] << ")\n";

  /*GLfloat* lightColors = new GLfloat[4 * NUM_LIGHTS];
  for (int i = 0; i < NUM_LIGHTS; i++) {
    glm::vec3 color = lightSources[i]->getColor();

    lightColors[i * 4] = color.x;
    lightColors[i * 4 + 1] = color.y;
    lightColors[i * 4 + 2] = color.z;
    lightColors[i * 4 + 3] = 1.f;
  }*/

  Shader* s = TinyGL::getInstance()->getShader("sPass");
  s->setUniform1i("u_numLights", NUM_LIGHTS);

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

  glGenTextures(1, &g_depthId);
  glBindTexture(GL_TEXTURE_2D, g_depthId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_NONE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, g_depthId, 0);

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
  Shader* g_fPass = new Shader("../Resources/ssao_fpass.vs", "../Resources/def_fpass.fs");
  g_fPass->bind();
  g_fPass->setUniformMatrix("viewMatrix", viewMatrix);
  g_fPass->setUniformMatrix("projMatrix", projMatrix);

  Shader* g_sPass = new Shader("../Resources/def_spass.vs", "../Resources/ssao.fs");
  g_sPass->bind();
  g_sPass->bindFragDataLoc("fColor", 0);
  g_sPass->setUniformMatrix("viewMatrix", viewMatrix);
  g_sPass->setUniformMatrix("projMatrix", glm::ortho(-1.f, 1.f, -1.f, 1.f));

  TinyGL::getInstance()->addResource(SHADER, "fPass", g_fPass);
  TinyGL::getInstance()->addResource(SHADER, "sPass", g_sPass);
}

void setupGeometry()
{
  Sphere** spheres;
  Quad* screenQuad;
  Cube** bottom_box;

  bottom_box = new Cube*[5];
  for(int i = 0; i < 5; i++) {
    bottom_box[i] = new Cube();
    bottom_box[i]->setDrawCb(drawByteIdx);
  }

  bottom_box[0]->setMaterialColor(glm::vec4(0.4, 0.6, 0.0, 1.0));
  bottom_box[1]->setMaterialColor(glm::vec4(0, 0.8, 0.0, 1.0));
  bottom_box[2]->setMaterialColor(glm::vec4(0.6, 0, 0.0, 1.0));
  bottom_box[3]->setMaterialColor(glm::vec4(0.4, 0.6, 0.9, 1.0));
  bottom_box[4]->setMaterialColor(glm::vec4(0, 0, 0.8, 1.0));

  bottom_box[0]->m_modelMatrix = glm::scale(glm::vec3(50, 0.1, 50));
  bottom_box[1]->m_modelMatrix = glm::scale(glm::vec3(50, 5, 0.3));
  bottom_box[2]->m_modelMatrix = glm::translate(glm::vec3(49.7, 0, 0)) * glm::scale(glm::vec3(0.3, 5, 50));
  bottom_box[3]->m_modelMatrix = glm::translate(glm::vec3(0, 0, 50)) * glm::scale(glm::vec3(50, 5, 0.3));
  bottom_box[4]->m_modelMatrix = glm::scale(glm::vec3(0.3, 5, 50));

  for(int i = 0; i < 5; i++) {
    bottom_box[i]->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * bottom_box[i]->m_modelMatrix));
    TinyGL::getInstance()->addResource(MESH, "bottom_box" + to_string(i), bottom_box[i]);
  }

  spheres = new Sphere*[NUM_SPHERES];
  for (int i = 0; i < NUM_SPHERES; i++) {
    spheres[i] = new Sphere(32, 32);
    spheres[i]->setDrawCb(drawSphere);
    spheres[i]->setMaterialColor(glm::vec4(1.0, 0.0, 0.0, 1.0));
  }

  for (int i = 0; i < W_SPHERES; i++) {
    for (int j = 0; j < H_SPHERES; j++) {
      spheres[i * W_SPHERES + j]->m_modelMatrix = glm::translate(glm::vec3((i+1) * 4, 1.5, (j+1) * 4)) * glm::scale(glm::vec3(1.5));
      spheres[i * W_SPHERES + j]->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * spheres[i * W_SPHERES + j]->m_modelMatrix));
    }
  }

  for (int i = 0; i < NUM_SPHERES; i++)
    TinyGL::getInstance()->addResource(MESH, "sphere" + to_string(i), spheres[i]);

  screenQuad = new Quad();
  screenQuad->setDrawCb(drawQuad);
  screenQuad->setMaterialColor(glm::vec4(0.f, 0.f, 0.f, 1.f));
  screenQuad->m_modelMatrix = glm::mat4(1.f);
  screenQuad->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * screenQuad->m_modelMatrix));
  TinyGL::getInstance()->addResource(MESH, "screenQuad", screenQuad);
}