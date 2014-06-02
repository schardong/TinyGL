#include "tglconfig.h"
#include "fcgt2config.h"
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
#include <sstream>

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

using namespace std;

void init(int argc, char** argv);
void initGLUT(int argc, char** argv);
void initGLEW();
void destroy();
void update();
void draw();
void reshape(int w, int h);
void keyPress(unsigned char c, int x, int y);
void specialKeyPress(int c, int x, int y);
void exit_cb();
void printInstructions();

int g_window = -1;

glm::mat4 viewMatrix;
glm::mat4 projMatrix;
glm::vec3 g_eye;
glm::vec3 g_center;

GLuint g_patternsTex[NUM_IMAGES];
GLuint g_cornersTex[NUM_IMAGES];
GLuint g_patternIdx = 0;
GLuint g_patternOff = 0;
bool g_showCorner = true;

bool initCalled = false;
bool initGLEWCalled = false;

void initPatterns(Detector detect, double thresh);
void drawQuad(size_t num_points);

int main(int argc, char** argv)
{
  Logger::getInstance()->setLogStream(&cout);
  Logger::getInstance()->log(TINYGL_LIBNAME + string(" v") + to_string(TINYGL_MAJOR_VERSION) + "." + to_string(TINYGL_MINOR_VERSION));
  
  initGLUT(argc, argv);
  initGLEW();
  init(argc, argv);

  glutMainLoop();
  return 0;
}

void initGLUT(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
  glutInitWindowSize(WINDOW_W, WINDOW_H);

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

void init(int argc, char** argv)
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

  Detector d = HARRIS;
  double thresh = 0.9;

  //Basic argument parsing.
  if(argc >= 3) {
    string param = string(argv[1]);
    if(param == "--shi-tomasi" || param == "-s")
      d = SHI_TOMASI;
    param = string(argv[2]);
    stringstream ss(param);
    ss >> thresh;
  } else if(argc == 2) {
    string param = string(argv[1]);
    if(param == "--shi-tomasi" || param == "-s")
      d = SHI_TOMASI;
  }

  initPatterns(d, thresh);
  printInstructions();

  Shader* g_shader = new Shader("../Resources/shaders/fcgt2.vs", "../Resources/shaders/fcgt2.fs");
  g_shader->bind();
  g_shader->bindFragDataLoc("fColor", 0);
  g_shader->setUniform1i("u_image", 0);
  TinyGL::getInstance()->addResource(SHADER, "fcgt2", g_shader);

  initCalled = true;
}

void destroy()
{
  TinyGL::getInstance()->freeResources();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDeleteTextures(NUM_IMAGES, g_patternsTex);
  glDeleteTextures(NUM_IMAGES, g_cornersTex);
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
  if(!g_showCorner)
    glBindTexture(GL_TEXTURE_2D, g_patternsTex[g_patternIdx]);
  else
    glBindTexture(GL_TEXTURE_2D, g_cornersTex[g_patternIdx]);

  s->setUniformMatrix("modelMatrix", glPtr->getMesh("quad")->m_modelMatrix);
  glPtr->getMesh("quad")->draw();

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
  WINDOW_W = w;
  WINDOW_H = h;
}

void keyPress(unsigned char c, int x, int y)
{
  int t = c - 49;
  if(t >= 0 && t <= 9) {
    if(g_patternOff == 0 && t < NUM_PATTERNS)
      g_patternIdx = t;
    else if((g_patternOff == NUM_PATTERNS && t < NUM_FID) || (g_patternOff == NUM_PATTERNS + NUM_FID && t < NUM_SOCCER))
      g_patternIdx = t + g_patternOff;
  }

  switch (c) {
  case ' ':
    g_showCorner = !g_showCorner;
    break;
  default:
    //printf("(%d, %d) = %d, %c\n", x, y, c, c);
    break;
  }

  string title = WINDOW_TITLE;
  if(g_patternOff == 0) title += " pattern";
  else if(g_patternOff == NUM_PATTERNS) title += " fiducial";
  else if(g_patternOff == NUM_PATTERNS + NUM_FID) title += " soccer_field";

  if((g_patternIdx - g_patternOff) < 10) title += "0";
  title += to_string(g_patternIdx - g_patternOff + 1);
  if(g_showCorner) title += " (corners)";
  glutSetWindowTitle((title).c_str());
}

void specialKeyPress(int c, int x, int y)
{
  switch (c) {
  case GLUT_KEY_F1:
    g_patternOff = 0;
    g_patternIdx = g_patternOff;
    break;
  case GLUT_KEY_F2:
    g_patternOff = NUM_PATTERNS;
    g_patternIdx = g_patternOff;
    break;
  case GLUT_KEY_F3:
    g_patternOff = NUM_PATTERNS + NUM_FID;
    g_patternIdx = g_patternOff;
    break;
  case GLUT_KEY_F10:
    printInstructions();
    break;
  default:
    printf("(%d, %d) = %d\n", x, y, c);
    break;
  }

  string title = WINDOW_TITLE;
  if(g_patternOff == 0) title += " pattern";
  else if(g_patternOff == NUM_PATTERNS) title += " fiducial";
  else if(g_patternOff == NUM_PATTERNS + NUM_FID) title += " soccer_field";

  if((g_patternIdx - g_patternOff) < 10) title += "0";
  title += to_string(g_patternIdx - g_patternOff + 1);
  if(g_showCorner) title += " (corners)";
  glutSetWindowTitle((title).c_str());
}

void exit_cb()
{
  glutDestroyWindow(g_window);
  destroy();
  exit(EXIT_SUCCESS);
}

void initPatterns(Detector detect, double thresh)
{
  Logger* log = Logger::getInstance();
  log->log("Initializing the patterns.");

  std::vector<Image*> patterns(NUM_IMAGES);

  //Reading the chessboard patterns.
  for(int i = 0; i < NUM_PATTERNS; i++) {
    string prefix = "../Resources/images/left";
    if(i < 10) prefix += "0";
    patterns[i] = imgGrey(imgReadBMP(const_cast<char*>((prefix + to_string(i+1) + ".bmp").c_str())));
    log->log("Loaded " + prefix + to_string(i+1) + ".bmp");
  }

  //Reading the fiducials.
  for(int i = 0; i < NUM_FID; i++) {
    string prefix = "../Resources/images/padrao0";
    patterns[i + NUM_PATTERNS] = imgGrey(imgReadBMP(const_cast<char*>((prefix + to_string(i+1) + ".bmp").c_str())));
    log->log("Loaded " + prefix + to_string(i+1) + ".bmp");
  }

  //Reading the soccer field.
  for(int i = 0; i < NUM_SOCCER; i++) {
    string prefix = "../Resources/images/soccer_field0";
    patterns[i + NUM_PATTERNS + NUM_FID] = imgGrey(imgReadBMP(const_cast<char*>((prefix + to_string(i+1) + ".bmp").c_str())));
    log->log("Loaded " + prefix + to_string(i+1) + ".bmp");
  }

  log->log("Done reading the input images.");

  //Detecting the corners.
  std::vector< std::vector<glm::vec2> > corner_values(NUM_IMAGES);
  std::vector<Image*> corners(NUM_IMAGES);
  for(int i = 0; i < NUM_IMAGES; i++) {
    int w = imgGetWidth(patterns[i]);
    int h = imgGetHeight(patterns[i]);
    corners[i] = imgCreate(w, h, 1);
    log->log("Finding corners of the image " + to_string(i + 1));
    corner_values[i] = HarrisCornerDetector(patterns[i], corners[i], detect, thresh);
    log->log(to_string(corner_values[i].size()) + " corners found.");
  }

  //Creating the textures to show the results.
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(NUM_IMAGES, g_patternsTex);
  for(int i = 0; i < NUM_IMAGES; i++) {
    int w = imgGetWidth(patterns[i]);
    int h = imgGetHeight(patterns[i]);
    float* pattern_data = imgGetData(patterns[i]);
    glBindTexture(GL_TEXTURE_2D, g_patternsTex[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, GL_FLOAT, pattern_data);
  }

  //Same as above.
  glGenTextures(NUM_IMAGES, g_cornersTex);
  for(int i = 0; i < NUM_IMAGES; i++) {
    int w = imgGetWidth(patterns[i]);
    int h = imgGetHeight(patterns[i]);
    float* corner_data = imgGetData(corners[i]);
    glBindTexture(GL_TEXTURE_2D, g_cornersTex[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, GL_FLOAT, corner_data);
  }
  glBindTexture(GL_TEXTURE_2D, 0);

  glutReshapeWindow(imgGetWidth(patterns[0]), imgGetHeight(patterns[0]));

  for(int i = 0; i < NUM_IMAGES; i++) {
    imgDestroy(patterns[i]);
    imgDestroy(corners[i]);
  }
}

void drawQuad(size_t num_points)
{
  glDrawElements(GL_TRIANGLE_STRIP, num_points, GL_UNSIGNED_BYTE, NULL);
}

void printInstructions()
{
  printf("---------------------------------------------------------------\n");
  printf("O programa inicia carregando as imagens de tabuleiros de xadrex localizadas em ../Resources/images/*.bmp\n");
  printf("A principio sao carregadas as imagens left*, padrao* e soccer_field*.\n");
  printf("Para trocar a imagem exibida, aperte um numero [1,9].\n");
  printf("Para trocar a imagem do padrao tabuleiro de xadrez para o fiducial, ou o campo de futebol, aperte F1, F2 ou F3.");
  printf("Para trocar o modo de exibicao da imagem original para a imagem que exibe os cantos detectados, aperte barra de espaco.\n");
  printf("Aperte F10 para exibir essas instrucoes novamente.\n");
  printf("Modo de uso: FCG-T2.exe [--harris,-h ou --shi-tomasi,-s] [threshold].\n");
  printf("O threshold eh uma valor numerico, ponto-flutuante entre 0 e 1.\n");
  printf("O valor de threshold soh pode ser especificado se o algoritmo tambem o for.\n");
  printf("Exemplo de uso:\n\tFCG-T2.exe --harris\n\tFCG-T2.exe -h 0.3\n\tFCG-T2.exe --shi-tomasi 0.86\n\tFCG-T2.exe -s\n");
  printf("Os valores padrao sao: --harris 0.9\n");
  printf("---------------------------------------------------------------\n");
}