#include "sphere.h"
#include "config.h"
#include <math.h>

#include <iostream>

Sphere::Sphere(int slices, int stacks)
{
  std::vector<GLfloat> vertices;
  for (int j = 0; j <= stacks; j++) {
    for (int i = 0; i <= slices; i++) {
      float theta = static_cast<float>((i / (float) slices) * 2 *  M_PI);
      float phi = static_cast<float>((j / (float)stacks) * M_PI);

      vertices.push_back(cos(theta) * sin(phi));
      vertices.push_back(cos(phi));
      vertices.push_back(sin(theta) * sin(phi));
    }
  }

  std::vector<GLfloat> normals = vertices;

  std::vector<GLuint> indices;
  for (int j = 0; j < slices; j++) {
    for (int i = 0; i < stacks; i++) {
      indices.push_back(i + j * (slices + 1));
      indices.push_back((i + 1) + (j + 1) * (slices + 1));
      indices.push_back(i + (j + 1) * (slices + 1));

      indices.push_back(i + j * (slices + 1));
      indices.push_back((i + 1) + j * (slices + 1));
      indices.push_back((i + 1) + (j + 1) * (slices + 1));
    }
  }

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
