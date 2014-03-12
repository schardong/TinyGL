#include "mesh.h"
#include "config.h"
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
  m_drawCb();
  glBindVertexArray(0);
}

Mesh* createGridMesh(int nx, int ny)
{
  size_t buff_size = nx * ny * 3;
  GLfloat* vertices = new GLfloat[buff_size];
  float h_step = static_cast<float>(1.f / nx);
  float v_step = static_cast<float>(1.f / ny);
  
  int k = 0;
  for (int i = 0; i < nx; i++) {
    for (int j = 0; j < ny; j++) {
      vertices[k++] = static_cast<float>(i * h_step);
      vertices[k++] = static_cast<float>(j * v_step);
      vertices[k++] = 0.f;
    }
  }

  GLfloat* colors = new GLfloat[buff_size];
  for (size_t i = 0; i < buff_size;) {
    colors[i++] = 1.0;
    colors[i++] = (float)rand() / (float)RAND_MAX;
    colors[i++] = 0.f;
  }

  k = 0;
  size_t idx_size = (nx - 1) * (ny - 1) * 6;
  GLuint* indices = new GLuint[idx_size];
  for (int i = 0; i < nx - 1; i++) {
    for (int j = 0; j < ny - 1; j++) {
      indices[k++] = i * nx + j;
      indices[k++] = (i + 1) * nx + j;
      indices[k++] = i * nx + (j + 1);
      indices[k++] = (i + 1) * nx + j;
      indices[k++] = (i + 1) * nx + (j + 1);
      indices[k++] = i * nx + (j + 1);
    }
  }

  //[TODO]: Finish the normals.
  GLfloat* normals = new GLfloat[buff_size];
  for (int i = 0; i < k; i += 3) {
    size_t r = indices[i];
    size_t s = indices[i + 1];
    size_t t = indices[i + 2];

    glm::vec3 o = glm::vec3(vertices[3 * r], vertices[3 * r + 1], vertices[3 * r + 2]);
    glm::vec3 a = glm::vec3(vertices[3 * s], vertices[3 * s + 1], vertices[3 * s + 2]);
    glm::vec3 b = glm::vec3(vertices[3 * t], vertices[3 * t + 1], vertices[3 * t + 2]);
    glm::vec3 normal = glm::normalize(glm::cross((a - o), (b - o)));

    normals[3 * r] = normal.x;
    normals[3 * r + 1] = normal.y;
    normals[3 * r + 2] = normal.z;

    normals[3 * s] = normal.x;
    normals[3 * s + 1] = normal.y;
    normals[3 * s + 2] = normal.z;

    normals[3 * t] = normal.x;
    normals[3 * t + 1] = normal.y;
    normals[3 * t + 2] = normal.z;
  }

  Mesh* mesh = new Mesh();

  BufferObject* vbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat) * buff_size, GL_STATIC_DRAW);
  vbuff->sendData(vertices);
  mesh->attachBuffer(vbuff);
  
  BufferObject* cbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat) * buff_size, GL_STATIC_DRAW);
  cbuff->sendData(colors);
  mesh->attachBuffer(cbuff);

  BufferObject* nbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat) * buff_size, GL_STATIC_DRAW);
  nbuff->sendData(normals);
  mesh->attachBuffer(nbuff);

  mesh->bind();

  BufferObject* ibuff = new BufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * idx_size, GL_STATIC_DRAW);
  ibuff->sendData(indices);
  mesh->attachBuffer(ibuff);

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

  delete[] vertices;
  delete[] colors;
  delete[] indices;
  delete[] normals;

  return mesh;
}

Mesh* createSphereMesh(int nx, int ny)
{
  float h_step = static_cast<float>(2 * M_PI) / nx;
  float v_step = static_cast<float>(M_PI) / ny;

  std::vector<GLfloat> vertices;
  
  int idx = 0;
  for (float phi = v_step; phi < M_PI; phi += v_step) {
    for (float theta = 0; theta < 2 * M_PI; theta += h_step) {
      vertices.push_back(cos(theta) * sin(phi));
      vertices.push_back(cos(phi));
      vertices.push_back(sin(theta) * sin(phi));
    }
  }

  vertices.push_back(0);
  vertices.push_back(1);
  vertices.push_back(0);

  vertices.push_back(0);
  vertices.push_back(-1);
  vertices.push_back(0);


  std::vector<GLfloat> colors(vertices.size());
  for (size_t i = 0; i < colors.size();) {
    colors[i++] = 0.0f;
    colors[i++] = 0.3f;
    colors[i++] = (float)rand() / (float)RAND_MAX;
  }

  std::vector<GLuint> indices;
  for (int i = 0; i < nx; i++) {
    for (int j = 0; j < ny; j++) {
      indices.push_back(i * nx + j);
      indices.push_back((i + 1) * nx + j);
      indices.push_back(i * nx + (j + 1));
      indices.push_back((i + 1) * nx + j);
      indices.push_back((i + 1) * nx + (j + 1));
      indices.push_back(i * nx + (j + 1));
    }
  }

  //GLfloat* normals = new GLfloat[buff_size];

  Mesh* mesh = new Mesh();
    
  BufferObject* vbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), GL_STATIC_DRAW);
  vbuff->sendData(&vertices[0]);
  mesh->attachBuffer(vbuff);

  BufferObject* cbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat) * colors.size(), GL_STATIC_DRAW);
  cbuff->sendData(&colors[0]);
  mesh->attachBuffer(cbuff);

  /*BufferObject* nbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat)* buff_size, GL_STATIC_DRAW);
  nbuff->sendData(normals);
  mesh->attachBuffer(nbuff);*/

  mesh->bind();

  BufferObject* ibuff = new BufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), GL_STATIC_DRAW);
  ibuff->sendData(&indices[0]);
  mesh->attachBuffer(ibuff);

  vbuff->bind();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);

  cbuff->bind();
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);

  /*nbuff->bind();
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(2);*/

  glBindVertexArray(0);

  //delete[] vertices;
  //delete[] colors;
  //delete[] indices;
  //delete[] normals;

  return mesh;
}