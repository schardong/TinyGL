#include "sphere.h"
#include "config.h"
#include <math.h>

#include <iostream>

Sphere::Sphere(int slices, int stacks)
{
  float h_step = static_cast<float>(2 * M_PI) / slices;
  float v_step = static_cast<float>(M_PI) / stacks;

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
  std::cout << "num_vertices = " << vertices.size()/3 << std::endl; 
  //TRIANGLES
  /*for (int i = 0; i < slices; i++) {
    for (int j = 0; j < stacks; j++) {
      indices.push_back(i * slices + j);
      indices.push_back((i + 1) * slices + j);
      indices.push_back(i * slices + (j + 1));
      indices.push_back((i + 1) * slices + j);
      indices.push_back((i + 1) * slices + (j + 1));
      indices.push_back(i * slices + (j + 1));
    }
  }*/

  //TRIANGLES-TOP
  for (int i = 1; i < slices; i++) {
    indices.push_back(0);
    indices.push_back(i);
    indices.push_back(i+1);
  }
  indices.push_back(0);
  indices.push_back(slices);
  indices.push_back(1);

  //TRIANGLES-MIDDLE
  /*for (int i = 2; i < vertices.size() / 3 - slices -1; i++) {
    if (i % slices == 0) {
      i++;
      continue;
    }
    indices.push_back(i - 1);
    indices.push_back(i + slices);
    indices.push_back(i);

    indices.push_back(i + slices);
    indices.push_back(i + slices + 1);
    indices.push_back(i);

  }*/

  //TRIANGLES-BOTTOM
  /*for (int i = slices * (stacks - 2) + 2; i <= slices * (stacks - 1); i++) {
    indices.push_back(vertices.size() / 3 - 1);
    indices.push_back(i - 1);
    indices.push_back(i);
  }
  indices.push_back(vertices.size() / 3 - 1);
  indices.push_back(slices * (stacks - 1));
  indices.push_back(slices * (stacks - 2) + 1);*/


  for (int i = slices * (stacks - 2) + 1; i < slices * (stacks - 1); i++) {
    indices.push_back(i);
    indices.push_back(i + 1);
    indices.push_back(slices * (stacks - 1) + 1);
  }
  indices.push_back(slices * (stacks - 2) + 1);
  indices.push_back(slices * (stacks - 1));
  indices.push_back(slices * (stacks - 1) + 1);

  
 /* std::cout << "\n---------vertices---------\n";
  for (int i = 0; i < vertices.size(); i += 3) {
    std::cout << vertices[i] << ", " << vertices[i + 1] << ", " << vertices[i + 2] << std::endl;
  }

  std::cout << "\n---------indices---------\n";
  for (int i = 0; i < indices.size(); i++) {
    std::cout << indices[i] << " ";
  }

  std::cout << "\n---------indexed vertices---------\n";
  for (int i = 0; i < indices.size(); i++) {
    std::cout << vertices[indices[i]] << ", " << vertices[indices[i] + 1] << ", " << vertices[indices[i] + 2] << std::endl;
  }*/

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
