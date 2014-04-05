#include "ciexyzmesh.h"

CIExyzMesh::CIExyzMesh(std::vector<glm::vec3> xyz)
{
  std::vector<GLfloat> vertices(xyz.size() * 3);

  for(size_t i = 0; i < vertices.size(); i += 3) {
    vertices[i] = xyz[i/3].x;
    vertices[i + 1] = xyz[i/3].y;
    vertices[i + 2] = xyz[i/3].z;
  }

  std::vector<GLfloat> colors(xyz.size() * 3);
  for(size_t i = 0; i < colors.size(); i += 3) {
    colors[i] = vertices[i] * 255;
    colors[i + 1] = vertices[i + 1] * 255;
    colors[i + 2] = vertices[i + 2] * 255;
  }

  std::vector<GLushort> indices;

  BufferObject* vbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), GL_STATIC_DRAW);
  vbuff->sendData(&vertices[0]);
  attachBuffer(vbuff);

  BufferObject* cbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat) * colors.size(), GL_STATIC_DRAW);
  cbuff->sendData(&colors[0]);
  attachBuffer(cbuff);

  bind();

  vbuff->bind();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);

  cbuff->bind();
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);

  m_numPoints = vertices.size() / 3;
  vertices.clear();
  colors.clear();
  indices.clear();
}


CIExyzMesh::~CIExyzMesh()
{
}
