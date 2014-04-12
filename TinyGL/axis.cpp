#include "axis.h"
#include "tglconfig.h"
#include <cstring>
#include <iostream>

Axis::Axis(glm::vec2 xlimits, glm::vec2 ylimits, glm::vec2 zlimits)
{
  std::vector<GLfloat> vertices(18);

  vertices[0] = xlimits.x;
  vertices[1] = 0;
  vertices[2] = 0;

  vertices[3] = xlimits.y;
  vertices[4] = 0;
  vertices[5] = 0;

  vertices[6] = 0;
  vertices[7] = ylimits.x;
  vertices[8] = 0;

  vertices[9] = 0;
  vertices[10] = ylimits.y;
  vertices[11] = 0;

  vertices[12] = 0;
  vertices[13] = 0;
  vertices[14] = zlimits.x;

  vertices[15] = 0;
  vertices[16] = 0;
  vertices[17] = zlimits.y;

  std::vector<GLfloat> colors(18);

  memset(&colors[0], 0, sizeof(GLfloat) * 18);
  colors[0] = colors[3] = colors[7] = colors[10] = colors[14] = colors[17] = 1.f;

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

  glBindVertexArray(0);
  m_numPoints = vertices.size() / 3;

  vertices.clear();
  colors.clear();
}

Axis::~Axis()
{
}
