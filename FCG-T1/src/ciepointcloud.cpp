#include "ciepointcloud.h"

CIEPointCloud::CIEPointCloud(std::vector<glm::vec3> points)
{
  std::vector<GLfloat> vertices(points.size() * 3);

  for (size_t i = 0; i < vertices.size(); i += 3) {
    vertices[i] = points[i / 3].x;
    vertices[i + 1] = points[i / 3].y;
    vertices[i + 2] = points[i / 3].z;
  }

  BufferObject* vbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat)* vertices.size(), GL_STATIC_DRAW);
  vbuff->sendData(&vertices[0]);
  attachBuffer(vbuff);

  BufferObject* cbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat)* vertices.size(), GL_STATIC_DRAW);
  cbuff->sendData(&vertices[0]);
  attachBuffer(cbuff);

  bind();

  vbuff->bind();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);

  cbuff->bind();
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
  m_numPoints = vertices.size() / 3;
  vertices.clear();
}

CIEPointCloud::CIEPointCloud(std::vector<glm::vec3> points, std::vector<glm::vec3> colors)
{
  std::vector<GLfloat> vertices(points.size() * 3);

  for (size_t i = 0; i < vertices.size(); i += 3) {
    vertices[i] = points[i / 3].x;
    vertices[i + 1] = points[i / 3].y;
    vertices[i + 2] = points[i / 3].z;
  }

  std::vector<GLfloat> color(points.size() * 3);

  for (size_t i = 0; i < vertices.size(); i += 3) {
    color[i] = colors[i / 3].x;
    color[i + 1] = colors[i / 3].y;
    color[i + 2] = colors[i / 3].z;
  }

  BufferObject* vbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat)* vertices.size(), GL_STATIC_DRAW);
  vbuff->sendData(&vertices[0]);
  attachBuffer(vbuff);

  BufferObject* cbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat)* vertices.size(), GL_STATIC_DRAW);
  cbuff->sendData(&color[0]);
  attachBuffer(cbuff);

  bind();

  vbuff->bind();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);

  cbuff->bind();
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
  m_numPoints = vertices.size() / 3;
  vertices.clear();
}

CIEPointCloud::~CIEPointCloud()
{
}
