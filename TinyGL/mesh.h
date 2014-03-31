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
  
protected:
  std::vector<BufferObject*> m_buffers;
  void(*m_drawCb)(size_t);

  GLuint m_vao;
  size_t m_numPoints;
};

#endif // MESH_H