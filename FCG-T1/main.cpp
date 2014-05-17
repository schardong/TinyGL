#include "tglconfig.h"
#include "fcgt1config.h"
#include "tinygl.h"
#include "logger.h"
#include "shader.h"
#include "mesh.h"
#include "grid.h"
#include "sphere.h"
#include "axis.h"
#include "ciemesh.h"
#include "ciepointcloud.h"
#include "colorspace.h"
#include "srgbmesh.h"

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

void initGamuts();
void initShader();

int g_window = -1;

glm::mat4 viewMatrix;
glm::mat4 projMatrix;
glm::vec3 g_eye;
glm::vec3 g_center;

bool initCalled = false;
bool initGLEWCalled = false;

bool g_wireRender = false;
bool g_meshRender = false;
bool g_pointRender = false;
int g_spaceRender = colorspace::CIEXYZ;
int g_spectrum = spectrum::SPECTRUM;

void drawPointsArrays(size_t num_points)
{
  glDrawArrays(GL_POINTS, 0, num_points);
}

void drawShortIdx(size_t num_points)
{
  glDrawElements(GL_TRIANGLES, num_points, GL_UNSIGNED_SHORT, NULL);
}

void drawByteIdx(size_t num_points)
{
  glDrawElements(GL_TRIANGLES, num_points, GL_UNSIGNED_BYTE, NULL);
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
  g_eye = glm::vec3(0.2, 0.2, 0.2);
  g_center = glm::vec3(0, 0, 0);
  viewMatrix = glm::lookAt(g_eye, g_center, glm::vec3(0, 1, 0));
  projMatrix = glm::perspective(static_cast<float>(M_PI / 4.f), 1.f, 0.1f, 1000.f);
  
  Axis* axis = new Axis(glm::vec2(-1, 1), glm::vec2(-1, 1), glm::vec2(-1, 1));
  axis->setDrawCb(drawAxis);
  axis->setMaterialColor(glm::vec4(0.f));
  axis->m_modelMatrix = glm::mat4(1.f);
  axis->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * axis->m_modelMatrix));
  TinyGL::getInstance()->addResource(MESH, "axis", axis);

  initGamuts();
  initShader();

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
  
  switch (g_spaceRender) {
  case colorspace::CIEXYZ:
    if (g_pointRender) {
      s->setUniformMatrix("modelMatrix", glPtr->getMesh("CIExyzCloud")->m_modelMatrix);
      glPtr->draw("CIExyzCloud");
    }
    if (g_meshRender) {
      s->setUniformMatrix("modelMatrix", glPtr->getMesh("CIExyzMesh")->m_modelMatrix);
      glPtr->draw("CIExyzMesh");
    }
    break;
  case colorspace::CIERGB:
    if (g_pointRender) {
      s->setUniformMatrix("modelMatrix", glPtr->getMesh("CIErgbCloud")->m_modelMatrix);
      glPtr->draw("CIErgbCloud");
    }
    if (g_meshRender) {
      s->setUniformMatrix("modelMatrix", glPtr->getMesh("CIErgbMesh")->m_modelMatrix); 
      glPtr->draw("CIErgbMesh");
    }
    break;
  case colorspace::CIELab:
    if (g_pointRender) {
      s->setUniformMatrix("modelMatrix", glPtr->getMesh("CIELabCloud")->m_modelMatrix);
      glPtr->draw("CIELabCloud");
    }
    if (g_meshRender) {
      s->setUniformMatrix("modelMatrix", glPtr->getMesh("CIELabMesh")->m_modelMatrix);
      glPtr->draw("CIELabMesh");
    }
    break;
  case colorspace::sRGB:
    if (g_pointRender) {
      s->setUniformMatrix("modelMatrix", glPtr->getMesh("CIEsRGBCloud")->m_modelMatrix);
      glPtr->draw("CIEsRGBCloud");
    }
    if (g_meshRender) {
      s->setUniformMatrix("modelMatrix", glPtr->getMesh("CIEsRGBMesh")->m_modelMatrix);
      glPtr->draw("CIEsRGBMesh");
    }
    break;
  }

  s->setUniformMatrix("modelMatrix", glPtr->getMesh("axis")->m_modelMatrix);
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
  projMatrix = glm::perspective(static_cast<float>(M_PI / 4.f), static_cast<float>(w) / static_cast<float>(h), 0.1f, 1000.f);

  Shader* s = TinyGL::getInstance()->getShader("fcgt1");
  s->bind();
  s->setUniformMatrix("projMatrix", projMatrix);

  Shader::unbind();
}

void keyPress(unsigned char c, int x, int y)
{
  SRGBMesh* m = (SRGBMesh*)TinyGL::getInstance()->getMesh("CIEsRGBMesh");
  switch (c) {
  case 'w':
    g_wireRender = !g_wireRender;
    break;
  case 'm':
    g_meshRender = !g_meshRender;
    break;
  case 'p':
    g_pointRender = !g_pointRender;
    break;
  case '1':
    m->setColorspace(colorspace::CIEXYZ);
    break;
  case '2':
    m->setColorspace(colorspace::CIERGB);
    break;
  case '3':
    m->setColorspace(colorspace::sRGB);
    break;
  case '4':
    m->setColorspace(colorspace::CIELab);
    break;
  }

  if(g_wireRender)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  else
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void specialKeyPress(int c, int x, int y)
{
  bool cameraChanged = false;

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
    g_spaceRender = colorspace::CIEXYZ;
    break;
  case GLUT_KEY_F2:
    g_spaceRender = colorspace::CIERGB;
    break;
  case GLUT_KEY_F3:
    g_spaceRender = colorspace::CIELab;
    break;
  case GLUT_KEY_F4:
    g_spaceRender = colorspace::sRGB;
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

void initGamuts()
{
  const int STEP = 1;
  std::vector<glm::vec3> xyz_cloud;
  std::vector<glm::vec3> xyz_mesh;
  std::vector<glm::vec3> rgb_cloud;
  std::vector<glm::vec3> rgb_mesh;
  std::vector<glm::vec3> lab_cloud;
  std::vector<glm::vec3> lab_mesh;
  std::vector<glm::vec3> srgb_cloud;
  std::vector<glm::vec3> srgb_mesh;

  std::vector<CIEPointCloud*> cieclouds(colorspace::n_colorspaces);
  std::vector<CIEMesh*> ciemesh(colorspace::n_colorspaces);

  //float* beta = new float[100000 * 400];
  float* beta = new float[400];
  float* illum = new float[400];
  std::vector<glm::vec3> xyzbar;

  memset(beta, 0, sizeof(float) * 400);
  for (int i = 0; i < 400; i++)  {
    float x, y, z;
    illum[i] = corGetD65(380.f + i);
    corGetCIExyz(380.f + i, &x, &y, &z);
    xyzbar.push_back(glm::vec3(x, y, z));
  }

  //for(int window = 1; window <= 400; window++) {
    for(int i = 0; i < 400; i++) {
      beta[i] = 1.f;
      glm::vec3 tmp;
      tmp = createCIEXYZ(beta, illum, xyzbar, STEP);
      xyz_cloud.push_back(tmp);
       glm::vec3 tmp2;
      corCIEXYZtoCIERGB(tmp.x, tmp.z, tmp.z, &tmp2.r, &tmp2.g, &tmp2.b);
      rgb_cloud.push_back(tmp2);

      corCIEXYZtoLab(tmp.x, tmp.y, tmp.z, &tmp2.r, &tmp2.g, &tmp2.b, D65);
      lab_cloud.push_back(tmp2);

      corCIEXYZtosRGB(tmp.x, tmp.y, tmp.z, &tmp2.r, &tmp2.g, &tmp2.b, D65);
      srgb_cloud.push_back(tmp2);
      beta[i] = 0.f;
    }

    for(int i = 0; i < 400; i++) {
      beta[i] = 1.f;
      if(i == 399)
        beta[0] = 1.f;
      else
        beta[i+1] = 1.f;
      glm::vec3 tmp;
      tmp = createCIEXYZ(beta, illum, xyzbar, STEP);
      xyz_cloud.push_back(tmp);
      glm::vec3 tmp2;
      corCIEXYZtoCIERGB(tmp.x, tmp.z, tmp.z, &tmp2.r, &tmp2.g, &tmp2.b);
      rgb_cloud.push_back(tmp2);

      corCIEXYZtoLab(tmp.x, tmp.y, tmp.z, &tmp2.r, &tmp2.g, &tmp2.b, D65);
      lab_cloud.push_back(tmp2);

      corCIEXYZtosRGB(tmp.x, tmp.y, tmp.z, &tmp2.r, &tmp2.g, &tmp2.b, D65);
      srgb_cloud.push_back(tmp2);
      beta[i] = 0.f;
      if(i == 399)
        beta[0] = 0.f;
      else
        beta[i+1] = 0.f;
    }
  //}

  /*FILE* fp = fopen("beta_reflectance_15000_1.dat", "rb");
  fread(beta, sizeof(float), 15000 * 400, fp);

  for (size_t i = 0; i < 15000; i++) {
    glm::vec3 tmp = createCIEXYZ(beta + i * 400, illum, xyzbar, STEP);
    xyz_cloud.push_back(tmp);

    glm::vec3 tmp2;
    corCIEXYZtoCIERGB(tmp.x, tmp.z, tmp.z, &tmp2.r, &tmp2.g, &tmp2.b);
    rgb_cloud.push_back(tmp2);

    corCIEXYZtoLab(tmp.x, tmp.y, tmp.z, &tmp2.r, &tmp2.g, &tmp2.b, D65);
    lab_cloud.push_back(tmp2);

    corCIEXYZtosRGB(tmp.x, tmp.y, tmp.z, &tmp2.r, &tmp2.g, &tmp2.b, D65);
    srgb_cloud.push_back(tmp2);
  }*/

  delete[] beta;
  delete[] illum;

  //variar o Y de 0 até 1
  //feito isso, interpolar os pontos gerados e formar uma malha de triangulos.
  const float Y_STEP = 0.1f;   
  float lwv[3];
  getReferenceWhite(lwv, D65);
  glm::vec3 lw(lwv[0], lwv[1], lwv[2]);
  float limit = glm::length(lw);// *glm::length(lw);

  for (float Y = Y_STEP; Y < limit; Y += Y_STEP) {

    for (int i = 0; i < 400; i += STEP) {
      float x, y, z, sum;
      corGetCIExyz(380.f + i, &x, &y, &z);
      sum = x + y + z;

      if (sum == 0)
        x = y = z = 0.f;
      else {
        x /= sum;
        y /= sum;
        z /= sum;
      }

      glm::vec3 ciexyz = glm::vec3(x, y, z) * Y;
      xyz_mesh.push_back(ciexyz);

      glm::vec3 ciergb;
      corCIEXYZtoCIERGB(ciexyz.x, ciexyz.y, ciexyz.z, &ciergb.r, &ciergb.g, &ciergb.b);
      rgb_mesh.push_back(ciergb);

      corCIEXYZtoLab(ciexyz.x, ciexyz.y, ciexyz.z, &ciergb.r, &ciergb.g, &ciergb.b, D65);
      lab_mesh.push_back(ciergb);

      corCIEXYZtosRGB(ciexyz.x, ciexyz.y, ciexyz.z, &ciergb.r, &ciergb.g, &ciergb.b, D65);
      srgb_mesh.push_back(ciergb);
    }

  }

  glm::vec3 tmp_lw(lw[0], lw[1], lw[2]);
  xyz_mesh.push_back(tmp_lw);

  float lw_rgb[3];
  corCIEXYZtoCIERGB(lw[0], lw[1], lw[2], &lw_rgb[0], &lw_rgb[1], &lw_rgb[2]);
  rgb_mesh.push_back(glm::vec3(lw_rgb[0], lw_rgb[1], lw_rgb[2]));

  corCIEXYZtoLab(lw[0], lw[1], lw[2], &lw_rgb[0], &lw_rgb[1], &lw_rgb[2], D65);
  lab_mesh.push_back(glm::vec3(lw_rgb[0], lw_rgb[1], lw_rgb[2]));

//  glm::vec3 tmp = CIEXYZtoCIEsRGB(tmp_lw, glm::vec3(lw_rgb[0], lw_rgb[1], lw_rgb[2]));
  srgb_mesh.push_back(tmp_lw);
    
  cieclouds[colorspace::CIEXYZ] = new CIEPointCloud(xyz_cloud);
  cieclouds[colorspace::CIEXYZ]->setDrawCb(drawPointsArrays);
  cieclouds[colorspace::CIEXYZ]->setMaterialColor(glm::vec4(0));
  cieclouds[colorspace::CIEXYZ]->m_modelMatrix = glm::mat4(1.f);
  cieclouds[colorspace::CIEXYZ]->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * cieclouds[colorspace::CIEXYZ]->m_modelMatrix));
  TinyGL::getInstance()->addResource(MESH, "CIExyzCloud", cieclouds[colorspace::CIEXYZ]);

  cieclouds[colorspace::CIERGB] = new CIEPointCloud(rgb_cloud);
  cieclouds[colorspace::CIERGB]->setDrawCb(drawPointsArrays);
  cieclouds[colorspace::CIERGB]->setMaterialColor(glm::vec4(0));
  cieclouds[colorspace::CIERGB]->m_modelMatrix = glm::mat4(1.f);
  cieclouds[colorspace::CIERGB]->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * cieclouds[colorspace::CIERGB]->m_modelMatrix));
  TinyGL::getInstance()->addResource(MESH, "CIErgbCloud", cieclouds[colorspace::CIERGB]);

  cieclouds[colorspace::CIELab] = new CIEPointCloud(lab_cloud);
  cieclouds[colorspace::CIELab]->setDrawCb(drawPointsArrays);
  cieclouds[colorspace::CIELab]->setMaterialColor(glm::vec4(0));
  cieclouds[colorspace::CIELab]->m_modelMatrix = glm::scale(glm::vec3(0.005f));
  cieclouds[colorspace::CIELab]->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * cieclouds[colorspace::CIELab]->m_modelMatrix));
  TinyGL::getInstance()->addResource(MESH, "CIELabCloud", cieclouds[colorspace::CIELab]);

  cieclouds[colorspace::sRGB] = new CIEPointCloud(srgb_cloud);
  cieclouds[colorspace::sRGB]->setDrawCb(drawPointsArrays);
  cieclouds[colorspace::sRGB]->setMaterialColor(glm::vec4(0));
  cieclouds[colorspace::sRGB]->m_modelMatrix = glm::mat4(1.f);
  cieclouds[colorspace::sRGB]->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * cieclouds[colorspace::sRGB]->m_modelMatrix));
  TinyGL::getInstance()->addResource(MESH, "CIEsRGBCloud", cieclouds[colorspace::sRGB]);

  ciemesh[colorspace::CIEXYZ] = new CIEMesh(xyz_mesh, srgb_mesh,limit / Y_STEP);
  ciemesh[colorspace::CIEXYZ]->setDrawCb(drawShortIdx);
  ciemesh[colorspace::CIEXYZ]->setMaterialColor(glm::vec4(0));
  ciemesh[colorspace::CIEXYZ]->m_modelMatrix = glm::mat4(1.f);
  ciemesh[colorspace::CIEXYZ]->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * ciemesh[colorspace::CIEXYZ]->m_modelMatrix));
  TinyGL::getInstance()->addResource(MESH, "CIExyzMesh", ciemesh[colorspace::CIEXYZ]);

  ciemesh[colorspace::CIERGB] = new CIEMesh(rgb_mesh, limit / Y_STEP);
  ciemesh[colorspace::CIERGB]->setDrawCb(drawShortIdx);
  ciemesh[colorspace::CIERGB]->setMaterialColor(glm::vec4(0));
  ciemesh[colorspace::CIERGB]->m_modelMatrix = glm::mat4(1.f);
  ciemesh[colorspace::CIERGB]->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * ciemesh[colorspace::CIERGB]->m_modelMatrix));
  TinyGL::getInstance()->addResource(MESH, "CIErgbMesh", ciemesh[colorspace::CIERGB]);

  ciemesh[colorspace::CIELab] = new CIEMesh(lab_mesh, srgb_mesh, limit / Y_STEP);
  ciemesh[colorspace::CIELab]->setDrawCb(drawShortIdx);
  ciemesh[colorspace::CIELab]->setMaterialColor(glm::vec4(0));
  ciemesh[colorspace::CIELab]->m_modelMatrix = glm::scale(glm::vec3(0.005f));
  ciemesh[colorspace::CIELab]->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * ciemesh[colorspace::CIELab]->m_modelMatrix));
  TinyGL::getInstance()->addResource(MESH, "CIELabMesh", ciemesh[colorspace::CIELab]);

//  ciemesh[colorspace::sRGB] = new CIEMesh(srgb_mesh, limit / Y_STEP);
//  ciemesh[colorspace::sRGB]->setDrawCb(drawShortIdx);
//  ciemesh[colorspace::sRGB]->setMaterialColor(glm::vec4(0));
//  ciemesh[colorspace::sRGB]->m_modelMatrix = glm::mat4(1.f);
//  ciemesh[colorspace::sRGB]->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * ciemesh[colorspace::sRGB]->m_modelMatrix));
//  TinyGL::getInstance()->addResource(MESH, "CIEsRGBMesh", ciemesh[colorspace::sRGB]);

  ciemesh[colorspace::sRGB] = new SRGBMesh();
  ciemesh[colorspace::sRGB]->setDrawCb(drawByteIdx);
  ciemesh[colorspace::sRGB]->setMaterialColor(glm::vec4(0));
  ciemesh[colorspace::sRGB]->m_modelMatrix = glm::mat4(1.f);
  ciemesh[colorspace::sRGB]->m_normalMatrix = glm::mat3(glm::inverseTranspose(viewMatrix * ciemesh[colorspace::sRGB]->m_modelMatrix));
  TinyGL::getInstance()->addResource(MESH, "CIEsRGBMesh", ciemesh[colorspace::sRGB]);
}

void initShader()
{
  Shader* g_shader = new Shader("../Resources/fcgt1.vs", "../Resources/fcgt1.fs");
  g_shader->bind();
  g_shader->bindFragDataLoc("out_vColor", 0);
  g_shader->setUniformMatrix("viewMatrix", viewMatrix);
  g_shader->setUniformMatrix("projMatrix", projMatrix);
  TinyGL::getInstance()->addResource(SHADER, "fcgt1", g_shader);
}