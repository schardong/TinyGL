#include "mesh.h"
#include "config.h"
#include <GL/glew.h>
#include <iostream>

#define GLM_FORCE_RADIANS
#include <glm/gtx/transform.hpp>

Mesh::Mesh() : m_vao(0)
{}

Mesh::Mesh(size_t num_elements, size_t bytes_per_element)
{}

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

void Mesh::createTriangleBuffer()
{
  GLfloat vertices[] = {
    -1.f, -1.f, 0.f,
    1.f, -1.f, 0.f,
    0.f, 1.f, 0.f
  };

  BufferObject* buff = new BufferObject(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), GL_STATIC_DRAW);
  buff->sendData(vertices);
  m_buffers.push_back(buff);
  
  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
}

void Mesh::createGridBuffer(int nx, int ny)
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

    normals[r] = normal.x;
    normals[r + 1] = normal.y;
    normals[r + 2] = normal.z;

    normals[s] = normal.x;
    normals[s + 1] = normal.y;
    normals[s + 2] = normal.z;

    normals[t] = normal.x;
    normals[t + 1] = normal.y;
    normals[t + 2] = normal.z;
  }

  BufferObject* vbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat) * buff_size, GL_STATIC_DRAW);
  vbuff->sendData(vertices);
  m_buffers.push_back(vbuff);
  
  BufferObject* cbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat) * buff_size, GL_STATIC_DRAW);
  cbuff->sendData(colors);
  m_buffers.push_back(cbuff);

  BufferObject* nbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat)* buff_size, GL_STATIC_DRAW);
  nbuff->sendData(normals);
  m_buffers.push_back(nbuff);

  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);

  BufferObject* ibuff = new BufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * idx_size, GL_STATIC_DRAW);
  ibuff->sendData(indices);
  m_buffers.push_back(ibuff);

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
}

void Mesh::createPointBuffer()
{
  GLfloat vertices[] = { 0.0, 0.0, 0.0 };
  GLfloat colors[] = { 1.0, 1.0, 1.0 };

  BufferObject* vbuff = new BufferObject(GL_ARRAY_BUFFER, 3 * sizeof(GLfloat), GL_STATIC_DRAW);
  vbuff->sendData(vertices);
  m_buffers.push_back(vbuff);

  BufferObject* cbuff = new BufferObject(GL_ARRAY_BUFFER, 3 * sizeof(GLfloat), GL_STATIC_DRAW);
  cbuff->sendData(colors);
  m_buffers.push_back(cbuff);

  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);

  vbuff->bind();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);

  cbuff->bind();
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

void Mesh::createSphereBuffer(int stacks, int slices)
{
  float h_step = static_cast<float>(2 * M_PI) / slices;
  float v_step = static_cast<float>(M_PI) / stacks;
  size_t buff_size = stacks * slices * 3;

  GLfloat* vertices = new GLfloat[buff_size];
  int idx = 0;

  for (float phi = 0; phi < M_PI; phi += v_step) {
    for (float theta = 0; theta < 2 * M_PI; theta += h_step) {
      vertices[idx++] = cos(theta) * sin(phi);
      vertices[idx++] = cos(phi);
      vertices[idx++] = sin(theta) * sin(phi);
    }
  }

  GLfloat* colors = new GLfloat[buff_size];
  for (size_t i = 0; i < buff_size;) {
    colors[i++] = 0.0f;
    colors[i++] = 0.8f;
    colors[i++] = 0.2f;
  }

  int k = 0;
  size_t idx_size = (stacks - 1) * (slices - 1) * 6;
  GLuint* indices = new GLuint[idx_size];
  for (int i = 0; i < stacks - 1; i++) {
    for (int j = 0; j < slices - 1; j++) {
      indices[k++] = i * stacks + j;
      indices[k++] = (i + 1) * stacks + j;
      indices[k++] = i * stacks + (j + 1);
      indices[k++] = (i + 1) * stacks + j;
      indices[k++] = (i + 1) * stacks + (j + 1);
      indices[k++] = i * stacks + (j + 1);
    }
  }
    
  BufferObject* vbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat) * buff_size, GL_STATIC_DRAW);
  vbuff->sendData(vertices);
  m_buffers.push_back(vbuff);

  BufferObject* cbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat) * buff_size, GL_STATIC_DRAW);
  cbuff->sendData(colors);
  m_buffers.push_back(cbuff);

  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);

  BufferObject* ibuff = new BufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * idx_size, GL_STATIC_DRAW);
  ibuff->sendData(indices);
  m_buffers.push_back(ibuff);

  vbuff->bind();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);

  cbuff->bind();
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  delete[] vertices;
  delete[] colors;
}

void Mesh::createAxesBuffer()
{
  GLfloat vertices[] = {
    0.0, 0.0, 0.0,
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0,
  };

  GLfloat colors[] = {
    0.0, 0.0, 0.0,
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0,
  };

  GLubyte indices[] = {
    0, 1,
    0, 2,
    0, 3
  };

  BufferObject* vbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat)* 12, GL_STATIC_DRAW);
  vbuff->sendData(vertices);
  m_buffers.push_back(vbuff);

  BufferObject* cbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat)* 12, GL_STATIC_DRAW);
  cbuff->sendData(colors);
  m_buffers.push_back(cbuff);

  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);

  BufferObject* ibuff = new BufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte)* 6, GL_STATIC_DRAW);
  ibuff->sendData(indices);
  m_buffers.push_back(ibuff);

  vbuff->bind();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);

  cbuff->bind();
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}