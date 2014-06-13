#include "mesh.h"
#include "tglconfig.h"
#include <GL/glew.h>
#include <iostream>

#define GLM_FORCE_RADIANS
#include <glm/gtx/transform.hpp>

Mesh::Mesh()
{
  glGenVertexArrays(1, &m_vao);
}

Mesh::~Mesh()
{
  for (unsigned int i = 0; i < m_buffers.size(); i++)
    delete m_buffers[i];
  m_buffers.clear();

  glDeleteVertexArrays(1, &m_vao);
}

void Mesh::attachBuffer(BufferObject* buff)
{
  m_buffers.push_back(buff);
}

void Mesh::draw()
{
  glBindVertexArray(m_vao);
  m_drawCb(m_numPoints);
  glBindVertexArray(0);
}
