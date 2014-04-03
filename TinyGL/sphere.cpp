#include "sphere.h"
#include "config.h"
#include <math.h>

#include <iostream>

Sphere::Sphere(int nx, int ny)
{
  float h_step = static_cast<float>(2 * M_PI) / nx;
  float v_step = static_cast<float>(M_PI) / ny;

  std::vector<GLfloat> vertices;

  vertices.push_back(0);
  vertices.push_back(1);
  vertices.push_back(0);

  for (float phi = v_step; phi < M_PI; phi += v_step) {
    for (float theta = 0; theta < 2 * M_PI; theta += h_step) {
      vertices.push_back(cos(theta) * sin(phi));
      vertices.push_back(cos(phi));
      vertices.push_back(sin(theta) * sin(phi));
    }
  }

  vertices.push_back(0);
  vertices.push_back(-1);
  vertices.push_back(0);

  std::vector<GLfloat> normals = vertices;

  std::vector<GLuint> indices;
  //TRIANGLES
  /*for (int i = 0; i < nx; i++) {
    for (int j = 0; j < ny; j++) {
      indices.push_back(i * nx + j);
      indices.push_back((i + 1) * nx + j);
      indices.push_back(i * nx + (j + 1));
      indices.push_back((i + 1) * nx + j);
      indices.push_back((i + 1) * nx + (j + 1));
      indices.push_back(i * nx + (j + 1));
    }
  }*/

  //TOP
  for (int i = 1; i <= nx; i++) {
    indices.push_back(0);
    indices.push_back(i);
  }
  indices.push_back(1);
  indices.push_back(nx);

  for (int i = 0; i < nx; i++) {
    for (int j = 0; j < ny; j++) {
      indices.push_back(i * nx + j);
      indices.push_back((i + 1) * nx + j);
    }
  }

  //BOTTOM
  for (int i = nx+1; i > 1; i--) {
    indices.push_back(vertices.size() / 3 - 1);
    indices.push_back(vertices.size() / 3 - i);
  }
  indices.push_back(vertices.size() / 3 - 1);
  indices.push_back(vertices.size() / 3 - nx - 1);

  BufferObject* vbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat)* vertices.size(), GL_STATIC_DRAW);
  vbuff->sendData(&vertices[0]);
  attachBuffer(vbuff);

  BufferObject* nbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat)* normals.size(), GL_STATIC_DRAW);
  nbuff->sendData(&normals[0]);
  attachBuffer(nbuff);

  bind();

  BufferObject* ibuff = new BufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* indices.size(), GL_STATIC_DRAW);
  ibuff->sendData(&indices[0]);
  attachBuffer(ibuff);

  vbuff->bind();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);
  
  nbuff->bind();
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  m_numPoints = indices.size();

  vertices.clear();
  indices.clear();
  normals.clear();
}

Sphere::~Sphere()
{
}
