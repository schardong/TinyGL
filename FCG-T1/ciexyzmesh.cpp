#include "ciexyzmesh.h"

CIExyzMesh::CIExyzMesh(std::vector<glm::vec3> xyz)
{
  std::vector<GLfloat> vertices((xyz.size() + 1) * 3); //+1*3 for the white point.

  float sum_x = 0;
  float sum_y = 0;
  float sum_z = 0;
  for(size_t i = 0; i < vertices.size() - 3; i += 3) {
    vertices[i] = xyz[i/3].x;
    vertices[i + 1] = xyz[i/3].y;
    vertices[i + 2] = xyz[i/3].z;
    sum_x += vertices[i];
    sum_y += vertices[i + 1];
    sum_z += vertices[i + 2];
  }

  vertices[vertices.size() - 3] = sum_x / xyz.size();
  vertices[vertices.size() - 2] = sum_y / xyz.size();
  vertices[vertices.size() - 1] = sum_z / xyz.size();

  //printf("-------------%f %f %f %d-----------\n", sum_x, sum_y, sum_z, xyz.size());
  //printf("-------------%f %f %f-----------\n", 1-vertices[vertices.size() - 3], 1-vertices[vertices.size() - 2], 1-vertices[vertices.size() - 1]);

  sum_x = 1 - vertices[vertices.size() - 3];
  sum_y = 1 - vertices[vertices.size() - 2];
  sum_z = 1 - vertices[vertices.size() - 1];

  std::vector<GLfloat> colors(vertices.size());
  for(size_t i = 0; i < colors.size(); i += 3) {
    colors[i] = vertices[i];
    colors[i + 1] = vertices[i + 1];
    colors[i + 2] = vertices[i + 2];
  }

  colors[colors.size() - 3] += sum_x;
  colors[colors.size() - 2] += sum_y;
  colors[colors.size() - 1] += sum_z;

  std::vector<GLushort> indices;
  for (int i = 0; i < vertices.size() / 3 - 1; i++) {
    indices.push_back(i);
    indices.push_back(i + 1);
    indices.push_back((vertices.size() / 3));
  }

  /*for (int i = 0; i < vertices.size() / 3 - 2; i++) {
    indices.push_back(i);
    indices.push_back(i + 1);
    indices.push_back((vertices.size() / 3));
  }*/

  BufferObject* vbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), GL_STATIC_DRAW);
  vbuff->sendData(&vertices[0]);
  attachBuffer(vbuff);

  BufferObject* cbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat) * colors.size(), GL_STATIC_DRAW);
  cbuff->sendData(&colors[0]);
  attachBuffer(cbuff);

  bind();

  BufferObject* ibuff = new BufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indices.size(), GL_STATIC_DRAW);
  ibuff->sendData(&indices[0]);
  attachBuffer(ibuff);

  vbuff->bind();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);

  cbuff->bind();
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);

  //m_numPoints = vertices.size() / 3;
  printf("INDICES SIZE = %d\n", indices.size());
  m_numPoints = indices.size();

  vertices.clear();
  colors.clear();
  indices.clear();
}


CIExyzMesh::~CIExyzMesh()
{
}
