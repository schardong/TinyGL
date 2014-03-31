#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class Shader
{
private:
  GLuint m_nProgId;
  GLuint m_nVertId;
  GLuint m_nFragId;
  GLuint m_nTessControlId;
  GLuint m_nTessEvalId;
  GLuint m_nGeomId;

  std::string m_sVertPath;
  std::string m_sFragPath;
  std::string m_sGeomPath;
  std::string m_sTessControlPath;
  std::string m_sTessEvalPath;

  const char *fileRead(const char *filename);
  GLuint compile(GLuint shaderType, const char *shaderCode);
  GLint getUniformLocation(std::string s);
  char* getShaderInfoLog(int id);
  char* getProgramInfoLog(int id, GLenum progVar);

public:
  Shader(std::string vertName,
    std::string fragName,
    std::string geomName = "",
    std::string tessControlName = "",
    std::string tessEvalName = "");

  ~Shader();

  inline GLuint getProgramId()
  {
    return m_nProgId;
  }

  void setUniformMatrix(std::string name, glm::mat4 m);
  void setUniformMatrix(std::string name, glm::mat3 m);
  void setUniform4fv(std::string name, glm::vec4 v);
  void setUniform1f(std::string name, float m);
  void setUniformfv(std::string name, float m[], int size);
  void setUniform1i(std::string name, int m);

  float* getUniformfv(std::string name, int size);
  int* getUniformiv(std::string name, int size);

  void bind();
  static void unbind();

  void bindFragDataLoc(std::string name, int layLoc);
};
#endif // SHADER_H
