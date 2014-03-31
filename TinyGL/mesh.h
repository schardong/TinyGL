#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "bufferobject.h"

class Mesh
{
public:
  glm::mat4 m_modelMatrix;
  glm::mat3 m_normalMatrix;

  Mesh();
  ~Mesh();

  void attachBuffer(BufferObject* buff);
  void draw();

  inline void setDrawCb(void(*drawCb)(size_t))
  {
    m_drawCb = drawCb;
  }

  inline void bind()
  {
    glBindVertexArray(m_vao);
  }

  void setMaterialColor(glm::vec4 rhs)
  {
    m_materialColor = rhs;
  }

  glm::vec4 getMaterialColor()
  {
    return m_materialColor;
  }
  
protected:
  std::vector<BufferObject*> m_buffers;
  void(*m_drawCb)(size_t);

  GLuint m_vao;
  glm::vec4 m_materialColor;
  size_t m_numPoints;
};

#endif // MESH_H