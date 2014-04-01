#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "bufferobject.h"

/**
 * class Mesh
 * This class is an abstraction of an mesh. It holds only the basic information
 * needed to draw one. The draw function is passed as a callback since this makes
 * the control easier. Ideally any simple mesh should inherit from this one and
 * define its own draw function.
 * The class also holds a series of buffers that store the information about it.
 * They may be buffers of any kind (vertex, color, normals, texture coordinates,
 * temparature, density, generation, cost, etc). All buffers are deleted and the
 * vector holding them is purged when the destructor is called.
 * The mesh also hold a material color, such attribute defines the material of the
 * mesh by holding an RGBA color.
 */
class Mesh
{
public:
  glm::mat4 m_modelMatrix;
  glm::mat3 m_normalMatrix;

  Mesh();
  virtual ~Mesh();

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