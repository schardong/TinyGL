#include "grid.h"

Grid::Grid(int nx, int ny) : Mesh()
{
  size_t buff_size = nx * ny * 3;
  std::vector<GLfloat> vertices;
  float h_step = static_cast<float>(1.f / nx);
  float v_step = static_cast<float>(1.f / ny);

  int k = 0;
  for (int i = 0; i < nx; i++) {
    for (int j = 0; j < ny; j++) {
      vertices.push_back(static_cast<float>(i * h_step));
      vertices.push_back(static_cast<float>(j * v_step));
      vertices.push_back(0.f);
    }
  }

  std::vector<GLfloat> colors;
  for (size_t i = 0; i < vertices.size(); i += 3) {
    colors.push_back(0.3f);
    colors.push_back(0.8f);
    colors.push_back(0.f);
  }

  std::vector<GLuint> indices;
  for (int i = 0; i < nx - 1; i++) {
    for (int j = 0; j < ny - 1; j++) {
      indices.push_back(i * nx + j);
      indices.push_back((i + 1) * nx + j);
      indices.push_back(i * nx + (j + 1));
      indices.push_back((i + 1) * nx + j);
      indices.push_back((i + 1) * nx + (j + 1));
      indices.push_back(i * nx + (j + 1));
    }
  }

  std::vector<GLfloat> normals;
  for (size_t i = 0; i < vertices.size(); i += 3) {
    normals.push_back(0);
    normals.push_back(1);
    normals.push_back(0);
  }

  BufferObject* vbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat)* buff_size, GL_STATIC_DRAW);
  vbuff->sendData(&vertices[0]);
  attachBuffer(vbuff);

  BufferObject* cbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat)* buff_size, GL_STATIC_DRAW);
  cbuff->sendData(&colors[0]);
  attachBuffer(cbuff);

  BufferObject* nbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat)* buff_size, GL_STATIC_DRAW);
  nbuff->sendData(&normals[0]);
  attachBuffer(nbuff);

  bind();

  BufferObject* ibuff = new BufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), GL_STATIC_DRAW);
  ibuff->sendData(&indices[0]);
  attachBuffer(ibuff);

  vbuff->bind();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);

  cbuff->bind();
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);

  nbuff->bind();
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);

  vertices.clear();
  colors.clear();
  indices.clear();
  normals.clear();
}

Grid::~Grid()
{
}
