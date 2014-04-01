#include <GL/glew.h>
#include "shader.h"
#include "logger.h"

#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Shader::Shader(std::string vertName,
  std::string fragName,
  std::string geomName,
  std::string tessControlName,
  std::string tessEvalName) :
  m_sVertPath(vertName),
  m_sFragPath(fragName),
  m_sGeomPath(geomName),
  m_sTessControlPath(tessControlName),
  m_sTessEvalPath(tessEvalName)
{
  m_nProgId = m_nVertId = m_nFragId = m_nTessControlId = m_nTessEvalId = m_nGeomId = 0;
  m_nProgId = glCreateProgram();

  if (!vertName.empty()) {
    m_nVertId = compile(GL_VERTEX_SHADER, fileRead(vertName.c_str()));
    glAttachShader(m_nProgId, m_nVertId);
  }
  if (!tessControlName.empty()) {
    m_nTessControlId = compile(GL_TESS_CONTROL_SHADER, fileRead(tessControlName.c_str()));
    glAttachShader(m_nProgId, m_nTessControlId);
  }
  if (!tessEvalName.empty()) {
    m_nTessEvalId = compile(GL_TESS_EVALUATION_SHADER, fileRead(tessEvalName.c_str()));
    glAttachShader(m_nProgId, m_nTessEvalId);
  }
  if (!geomName.empty()) {
    m_nGeomId = compile(GL_GEOMETRY_SHADER, fileRead(geomName.c_str()));
    glAttachShader(m_nProgId, m_nGeomId);
  }
  if (!fragName.empty()) {
    m_nFragId = compile(GL_FRAGMENT_SHADER, fileRead(fragName.c_str()));
    glAttachShader(m_nProgId, m_nFragId);
  }

  glLinkProgram(m_nProgId);

  GLint linked;
  glGetProgramiv(m_nProgId, GL_LINK_STATUS, &linked);
  
  if (!linked) {
    GLint len;
    glGetProgramiv(m_nProgId, GL_INFO_LOG_LENGTH, &len);
    char* msg = (char*)calloc(len, sizeof(char));
    glGetProgramInfoLog(m_nProgId, len, 0, msg);
    Logger::getInstance()->error(msg);
    free(msg);
  }
}

Shader::~Shader()
{
  if (m_nVertId != 0) {
    glDetachShader(m_nProgId, m_nVertId);
    glDeleteShader(m_nVertId);
  }
  if (m_nFragId != 0) {
    glDetachShader(m_nProgId, m_nFragId);
    glDeleteShader(m_nFragId);
  }
  if (m_nGeomId != 0) {
    glDetachShader(m_nProgId, m_nGeomId);
    glDeleteShader(m_nGeomId);
  }
  if (m_nTessControlId != 0) {
    glDetachShader(m_nProgId, m_nTessControlId);
    glDeleteShader(m_nTessControlId);
  }
  if (m_nTessEvalId != 0) {
    glDetachShader(m_nProgId, m_nTessEvalId);
    glDeleteShader(m_nTessEvalId);
  }
  glDeleteProgram(m_nProgId);
}

void Shader::setUniformMatrix(std::string name, glm::mat4 m)
{
  GLint loc = getUniformLocation(name.c_str());
  if (loc == -1) {
    //std::cerr << "ERROR: The variable name " << name << " was not set because its location was not found on the shader program " << getProgramId() << std::endl;
    //std::cerr << "In files-> Vertex shader: " << m_sVertPath
    //          << " Fragment shader: " << m_sFragPath
    //          << " Geometry shader: " << m_sGeomPath
    //          << " Tesselation control shader: " << m_sTessControlPath
    //          << " Tesselation evaluation shader: " << m_sTessEvalPath
    //          << std::endl;
    return;
  }
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}

void Shader::setUniformMatrix(std::string name, glm::mat3 m)
{
  GLint loc = getUniformLocation(name.c_str());
  if (loc == -1) {
    return;
  }
  glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}

void Shader::setUniform4fv(std::string name, glm::vec4 v)
{
  GLint loc = getUniformLocation(name.c_str());
  if (loc == -1) {
    //            std::cerr << "ERROR: The variable name " << name << " was not set because its location was not found on the shader program " << getProgramId() << std::endl;
    //            std::cerr << "In files-> Vertex shader: " << m_sVertPath
    //                      << " Fragment shader: " << m_sFragPath
    //                      << " Geometry shader: " << m_sGeomPath
    //                      << " Tesselation control shader: " << m_sTessControlPath
    //                      << " Tesselation evaluation shader: " << m_sTessEvalPath
    //                      << std::endl;
    return;
  }
  glUniform4fv(loc, 1, glm::value_ptr(v));
}

void Shader::setUniformfv(std::string name, float m[], int size)
{
  GLint loc = getUniformLocation(name.c_str());
  if (loc == -1) {
    //            std::cerr << "ERROR: The variable name " << name << " was not set because its location was not found on the shader program " << getProgramId() << std::endl;
    //            std::cerr << "In files-> Vertex shader: " << m_sVertPath
    //                      << " Fragment shader: " << m_sFragPath
    //                      << " Geometry shader: " << m_sGeomPath
    //                      << " Tesselation control shader: " << m_sTessControlPath
    //                      << " Tesselation evaluation shader: " << m_sTessEvalPath
    //                      << std::endl;
    return;
  }
  switch (size) {
  case 4:
    glUniform4f(loc, m[0], m[1], m[2], m[3]);
    break;
  case 3:
    glUniform3f(loc, m[0], m[1], m[2]);
    break;
  case 2:
    glUniform2f(loc, m[0], m[1]);
    break;
  case 1:
    glUniform1f(loc, m[0]);
    break;
  }
}

void Shader::setUniform1f(std::string name, float m)
{
  GLint loc = getUniformLocation(name.c_str());
  if (loc == -1) {
    //            std::cerr << "ERROR: The variable name " << name << " was not set because its location was not found on the shader program " << getProgramId() << std::endl;
    //            std::cerr << "In files-> Vertex shader: " << m_sVertPath
    //                      << " Fragment shader: " << m_sFragPath
    //                      << " Geometry shader: " << m_sGeomPath
    //                      << " Tesselation control shader: " << m_sTessControlPath
    //                      << " Tesselation evaluation shader: " << m_sTessEvalPath
    //                      << std::endl;
    return;
  }
  glUniform1f(loc, m);
}

void Shader::setUniform1i(std::string name, int m)
{
  GLint loc = getUniformLocation(name.c_str());
  if (loc == -1) {
    //            std::cerr << "ERROR: The variable name " << name << " was not set because its location was not found on the shader program " << getProgramId() << std::endl;
    //            std::cerr << "In files-> Vertex shader: " << m_sVertPath
    //                      << " Fragment shader: " << m_sFragPath
    //                      << " Geometry shader: " << m_sGeomPath
    //                      << " Tesselation control shader: " << m_sTessControlPath
    //                      << " Tesselation evaluation shader: " << m_sTessEvalPath
    //                      << std::endl;
    return;
  }
  glUniform1i(loc, m);
}

float* Shader::getUniformfv(std::string name, int size)
{
  if (name.empty() || size <= 0) {
    std::cerr << "ERROR: getUniform -> Invalid parameter(s)" << std::endl;
    return NULL;
  }
  float* params = new float[size];
  GLint loc = getUniformLocation(name);
  glGetUniformfv(getProgramId(), loc, params);
  return params;
}

int* Shader::getUniformiv(std::string name, int size)
{
  if (name.empty() || size <= 0) {
    std::cerr << "ERROR: getUniform -> Invalid parameter(s)" << std::endl;
    return NULL;
  }
  int* params = new int[size];
  GLint loc = getUniformLocation(name);
  glGetUniformiv(getProgramId(), loc, params);
  return params;
}

void Shader::bind()
{
  glUseProgram(getProgramId());
}

void Shader::unbind()
{
  glUseProgram(0);
}

void Shader::validate()
{
  glValidateProgram(m_nProgId);
}

void Shader::bindFragDataLoc(std::string name, int layLoc)
{
  glBindFragDataLocation(getProgramId(), layLoc, name.c_str());
}

const char* Shader::fileRead(const char* filename)
{
  FILE* fp;
  char* content = NULL;
  int count = 0;

  if (filename != NULL) {
    fp = fopen(filename, "rt");

    if (fp != NULL) {
      fseek(fp, 0, SEEK_END);
      count = ftell(fp);
      rewind(fp);

      if (count > 0) {
        content = new char[count + 1];
        count = (int)fread(content, sizeof(char), count, fp);
        content[count] = '\0';
      }
      fclose(fp);
    }
    else
      std::cerr << "ERROR: File " << filename << " not found!" << std::endl;
  }
  else
    std::cerr << "ERROR: Shader filename is NULL!" << std::endl;

  return content;
}

GLuint Shader::compile(GLuint shaderType, const char* shaderCode)
{
  if (shaderType != GL_VERTEX_SHADER &&
    shaderType != GL_FRAGMENT_SHADER &&
    shaderType != GL_GEOMETRY_SHADER &&
    shaderType != GL_TESS_CONTROL_SHADER &&
    shaderType != GL_TESS_EVALUATION_SHADER)
    return 0;

  GLuint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &shaderCode, NULL);
  glCompileShader(shader);

  // Error checking.
  GLint compiled;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

  if (!compiled) {
    Logger::getInstance()->error(m_sVertPath + " " + m_sFragPath + " " + m_sGeomPath + " " + m_sTessControlPath + " " + m_sTessEvalPath + "\n  " + getShaderInfoLog(shader));
    return false;
  }

  return shader;
}

char *Shader::getShaderInfoLog(int id)
{
  int len;
  glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
  char* log = new char[len];
  glGetShaderInfoLog(id, len, &len, &log[0]);
  return log;
}

GLint Shader::getUniformLocation(std::string s)
{
  GLint loc = glGetUniformLocation(getProgramId(), s.c_str());
  return loc;
}
