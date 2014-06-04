#include "tglconfig.h"
#include "tinygl.h"
#include "logger.h"
#include "shader.h"
#include "mesh.h"
#include "cube.h"
#include "sphere.h"
#include "quad.h"
#include "light.h"

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

static const int W_SPHERES = 10;
static const int H_SPHERES = 10;
static const int NUM_SPHERES = W_SPHERES * H_SPHERES;
static const int NUM_LIGHTS = 1;
static int WINDOW_W = 700;
static int WINDOW_H = 700;

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

GLuint g_colorId[num_buffers];
GLuint g_depthId;
GLuint g_blurColorId;
GLuint g_rndNormalId;

void resendShaderUniforms();
void setupLights();
void setupFBO(GLuint w, GLuint h);
void setupShaders();
void setupGeometry();

void drawSphere(size_t num_points)
{
  glDrawElements(GL_TRIANGLES, num_points, GL_UNSIGNED_INT, NULL);
}

void drawArrays(size_t num_points)
{
  glDrawArrays(GL_TRIANGLES, 0, num_points);
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
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

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

  Image* rnd_normal = imgReadBMP("../Resources/noise.bmp");
  
  glGenTextures(1, &g_rndNormalId);
  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, g_rndNormalId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgGetWidth(rnd_normal), imgGetWidth(rnd_normal), 0, GL_RGB, GL_UNSIGNED_BYTE, imgGetData(rnd_normal));

  resendShaderUniforms();

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
  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_2D, 0);

  glDeleteTextures(num_buffers, g_colorId);
  glDeleteTextures(1, &g_depthId);
  glDeleteTextures(1, &g_blurColorId);
  glDeleteTextures(1, &g_rndNormalId);
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

  TinyGL* glPtr = TinyGL::getInstance();

  //First pass. Filling the geometry buffers.
  glPtr->getFBO("SSAO_FBO")->bind(GL_FRAMEBUFFER);

  GLenum drawBuffer[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
  glDrawBuffers(num_buffers, drawBuffer);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;

  Shader* s = glPtr->getShader("fPass");
  s->bind();

  for (int i = 0; i < NUM_SPHERES; i++) {
    std::string obj_name = "sphere" + to_string(i);
    s->setUniformMatrix("modelMatrix", TinyGL::getInstance()->getMesh(obj_name)->m_modelMatrix);
    s->setUniformMatrix("normalMatrix", TinyGL::getInstance()->getMesh(obj_name)->m_normalMatrix);
    s->setUniform4fv("u_materialColor", TinyGL::getInstance()->getMesh(obj_name)->getMaterialColor());
    glPtr->draw(obj_name);
  }

  for(int i = 0; i < 5; i++) {
    std::string obj_name = "bottom_box" + to_string(i);
    s->setUniformMatrix("modelMatrix", TinyGL::getInstance()->getMesh(obj_name)->m_modelMatrix);
    s->setUniformMatrix("normalMatrix", TinyGL::getInstance()->getMesh(obj_name)->m_normalMatrix);
    s->setUniform4fv("u_materialColor", TinyGL::getInstance()->getMesh(obj_name)->getMaterialColor());
    glPtr->draw(obj_name);
  }

  //Second pass. SSAO is calculated here.
  glDisable(GL_DEPTH_TEST);

  drawBuffer[0] = GL_COLOR_ATTACHMENT3;
  glDrawBuffers(1, &drawBuffer[0]);

  glClear(GL_COLOR_BUFFER_BIT);

  s = glPtr->getShader("sPass");
  s->bind();

  glPtr->draw("screenQuad");

  FramebufferObject::unbind();

  //Third pass. Blurring the results.
  glClear(GL_COLOR_BUFFER_BIT);

  s = glPtr->getShader("tPass");
  s->bind();

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

  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_2D, g_blurColorId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, w, h, 0, GL_RED, GL_FLOAT, NULL);

  glViewport(0, 0, w, h);
  projMatrix = glm::perspective(static_cast<float>(M_PI / 4.f), static_cast<float>(w) / static_cast<float>(h), 1.f, 100.f);

  resendShaderUniforms();

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
  case 32: //SPACEBAR
    Shader::unbind();

    Shader* fPass = TinyGL::getInstance()->getShader("fPass");
    Shader* sPass = TinyGL::getInstance()->getShader("sPass");
    Shader* tPass = TinyGL::getInstance()->getShader("tPass");
    delete fPass;
    delete sPass;
    delete tPass;

    fPass = new Shader("../Resources/shaders/ssao_fpass.vs", "../Resources/shaders/def_fpass.fs");
    sPass = new Shader("../Resources/shaders/def_spass.vs", "../Resources/shaders/ssao.fs");
    tPass = new Shader("../Resources/shaders/def_spass.vs", "../Resources/shaders/blur.fs");

    TinyGL::getInstance()->addResource(SHADER, "fPass", fPass);
    TinyGL::getInstance()->addResource(SHADER, "sPass", sPass);
    TinyGL::getInstance()->addResource(SHADER, "tPass", tPass); 

    resendShaderUniforms();
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

void resendShaderUniforms()
{
  Mesh* quad = TinyGL::getInstance()->getMesh("screenQuad");
  Shader* fPass = TinyGL::getInstance()->getShader("fPass");
  Shader* sPass = TinyGL::getInstance()->getShader("sPass");
  Shader* tPass = TinyGL::getInstance()->getShader("tPass");

  fPass->bind();
  fPass->setUniformMatrix("viewMatrix", viewMatrix);
  fPass->setUniformMatrix("projMatrix", projMatrix);

  sPass->bind();
  sPass->bindFragDataLoc("fColor", 0);
  sPass->setUniformMatrix("projMatrix", glm::ortho(-1.f, 1.f, -1.f, 1.f));
  sPass->setUniformMatrix("modelMatrix", quad->m_modelMatrix);
  sPass->setUniform4fv("u_materialColor", quad->getMaterialColor());
  sPass->setUniform1f("u_zNear", 1.f);
  sPass->setUniform1f("u_zFar", 100.f);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, g_colorId[MATERIAL]);
  sPass->setUniform1i("u_diffuseMap", 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, g_colorId[NORMAL]);
  sPass->setUniform1i("u_normalMap", 1);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, g_colorId[VERTEX]);
  sPass->setUniform1i("u_vertexMap", 2);

  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, g_depthId);
  sPass->setUniform1i("u_depthMap", 4);

  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, g_rndNormalId);
  sPass->setUniform1i("u_rndNormalMap", 5);

  float ss[2] = {WINDOW_W, WINDOW_H};
  sPass->setUniformfv("u_screenSize", ss, 2);

  tPass->bind();
  tPass->bindFragDataLoc("fColor", 0);
  tPass->setUniformMatrix("projMatrix", glm::ortho(-1.f, 1.f, -1.f, 1.f));
  tPass->setUniformMatrix("modelMatrix", quad->m_modelMatrix);
  tPass->setUniform4fv("u_materialColor", quad->getMaterialColor());
  
  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_2D, g_blurColorId);
  tPass->setUniform1i("u_ssaoMap", 6);

}

void setupLights()
{
  Light** lightSources = new Light*[NUM_LIGHTS];

  for (int i = 0; i < NUM_LIGHTS; i++) {
    lightSources[i] = new Light();
    lightSources[i]->setPosition(glm::vec3(rand() % 35, 5 + rand() % 10, rand() % 35));
    lightSources[i]->setColor(glm::vec3((float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX));
  }

  GLfloat* lightCoords = new GLfloat[4 * NUM_LIGHTS];
  for (int i = 0; i < NUM_LIGHTS; i++) {
    glm::vec3 pos = lightSources[i]->getPosition();

    lightCoords[i * 4] = pos.x;
    lightCoords[i * 4 + 1] = pos.y;
    lightCoords[i * 4 + 2] = pos.z;
    lightCoords[i * 4 + 3] = 1.f;
  }

  Shader* s = TinyGL::getInstance()->getShader("sPass");
  s->setUniform1i("u_numLights", NUM_LIGHTS);

  GLuint idxPos = glGetUniformBlockIndex(s->getProgramId(), "LightPos");
  glUniformBlockBinding(s->getProgramId(), idxPos, 0);

  BufferObject* ubuffLightPos = new BufferObject(GL_UNIFORM_BUFFER, sizeof(GLfloat)* 3 * NUM_LIGHTS, GL_STATIC_DRAW);
  ubuffLightPos->sendData(lightCoords);

  glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubuffLightPos->getId());
  TinyGL::getInstance()->addResource(BUFFER, "lightpos_buff", ubuffLightPos);

  delete lightCoords;
}

void setupFBO(GLuint w, GLuint h)
{
  FramebufferObject* fbo = new FramebufferObject();
  fbo->bind(GL_FRAMEBUFFER);

  glGenTextures(num_buffers, g_colorId);

  for (int i = 0; i < num_buffers; i++) {
    glBindTexture(GL_TEXTURE_2D, g_colorId[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    fbo->attachTexBuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, g_colorId[i], 0);
  }

  glGenTextures(1, &g_blurColorId);
  glBindTexture(GL_TEXTURE_2D, g_blurColorId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, w, h, 0, GL_RED, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  fbo->attachTexBuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, g_blurColorId, 0);

  glGenTextures(1, &g_depthId);
  glBindTexture(GL_TEXTURE_2D, g_depthId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_NONE);
  fbo->attachTexBuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, g_depthId, 0);

  fbo->checkStatus();

  FramebufferObject::unbind();
  TinyGL::getInstance()->addResource(FRAMEBUFFER, "SSAO_FBO", fbo);
}

void setupShaders()
{
  Shader* fPass = new Shader("../Resources/shaders/ssao_fpass.vs", "../Resources/shaders/def_fpass.fs");
  Shader* sPass = new Shader("../Resources/shaders/def_spass.vs", "../Resources/shaders/ssao.fs");
  Shader* tPass = new Shader("../Resources/shaders/def_spass.vs", "../Resources/shaders/blur.fs");

  Shader::unbind();

  TinyGL::getInstance()->addResource(SHADER, "fPass", fPass);
  TinyGL::getInstance()->addResource(SHADER, "sPass", sPass);
  TinyGL::getInstance()->addResource(SHADER, "tPass", tPass);
}

void setupGeometry()
{
  Sphere** spheres;
  Quad* screenQuad;
  Cube** bottom_box;

  bottom_box = new Cube*[5];
  for(int i = 0; i < 5; i++) {
    bottom_box[i] = new Cube();
    bottom_box[i]->setDrawCb(drawArrays);
  }

  bottom_box[0]->setMaterialColor(glm::vec4(0.4, 0.6, 0.0, 1.0));
  bottom_box[1]->setMaterialColor(glm::vec4(0, 0.8, 0.0, 1.0));
  bottom_box[2]->setMaterialColor(glm::vec4(0.6, 0, 0.0, 1.0));
  bottom_box[3]->setMaterialColor(glm::vec4(0.4, 0.6, 0.9, 1.0));
  bottom_box[4]->setMaterialColor(glm::vec4(0, 0, 0.8, 1.0));

  bottom_box[0]->m_modelMatrix = glm::translate(glm::vec3(25, 0, 25)) * glm::scale(glm::vec3(50, 0.1, 50));
  bottom_box[1]->m_modelMatrix = glm::translate(glm::vec3(25, 2.5, 0)) * glm::scale(glm::vec3(50, 5, 0.3));
  bottom_box[2]->m_modelMatrix = glm::translate(glm::vec3(50, 2.5, 25)) * glm::scale(glm::vec3(0.3, 5, 50));
  bottom_box[3]->m_modelMatrix = glm::translate(glm::vec3(25, 2.5, 50)) * glm::scale(glm::vec3(50, 5, 0.3));
  bottom_box[4]->m_modelMatrix = glm::translate(glm::vec3(0, 2.5, 25)) * glm::scale(glm::vec3(0.3, 5, 50));

  for(int i = 0; i < 5; i++) {
    bottom_box[i]->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * bottom_box[i]->m_modelMatrix));
    TinyGL::getInstance()->addResource(MESH, "bottom_box" + to_string(i), bottom_box[i]);
  }

  spheres = new Sphere*[NUM_SPHERES];
  for (int i = 0; i < NUM_SPHERES; i++) {
    spheres[i] = new Sphere(60, 60);
    spheres[i]->setDrawCb(drawSphere);
    spheres[i]->setMaterialColor(glm::vec4(1.0, 0.0, 0.0, 1.0));
  }

  for (int i = 0; i < W_SPHERES; i++) {
    for (int j = 0; j < H_SPHERES; j++) {
      spheres[i * W_SPHERES + j]->m_modelMatrix = glm::translate(glm::vec3((i+1) * 4, 1.6, (j+1) * 4)) * glm::scale(glm::vec3(1.5));
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