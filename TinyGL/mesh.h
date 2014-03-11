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
  glm::mat4 m_normalMatrix;

  Mesh();
  Mesh(size_t num_elements, size_t bytes_per_element);
  ~Mesh();

  void attachBuffer(BufferObject* buff);
  void draw();

  inline void setDrawCb(void(*drawCb)())
  {
    m_drawCb = drawCb;
  }

  inline void bind()
  {
    glBindVertexArray(m_vao);
  }

  void createTriangleBuffer();
  void createPointBuffer();
  void createGridBuffer(int nx, int ny);
  void createSphereBuffer(int stacks, int slices);
  void createAxesBuffer();

protected:
  std::vector<BufferObject*> m_buffers;
  void(*m_drawCb)();

  GLuint m_vao;
};

#endif // MESH_H