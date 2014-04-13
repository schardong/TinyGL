#include "srgbmesh.h"
#include <cstring>

SRGBMesh::SRGBMesh()
{
  GLfloat vertices[] = {
    0, 0, 0,
    1, 0, 0,
    1, 1, 0,
    0, 1, 0,
    0, 0, 1,
    1, 0, 1,
    1, 1, 1,
    0, 1, 1
  };

  GLfloat colors[24];
  memcpy(colors, vertices, sizeof(GLfloat) * 24);

  GLbyte indices[] {
    //BACK
    0, 1, 2,
    0, 2, 3,
    //FRONT
    4, 5, 6,
    4, 6, 7,
    //RIGHT
    5, 1, 2,
    5, 2, 6,
    //LEFT
    0, 4, 7,
    0, 7, 3,
    //TOP
    7, 6, 2,
    7, 2, 3,
    //BOTTOM
    0, 1, 5,
    0, 5, 4
  };

  BufferObject* vbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24, GL_STATIC_DRAW);
  vbuff->sendData(vertices);
  attachBuffer(vbuff);

  BufferObject* cbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24, GL_STATIC_DRAW);
  cbuff->sendData(colors);
  attachBuffer(cbuff);

  bind();

  BufferObject* ibuff = new BufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 36, GL_STATIC_DRAW);
  ibuff->sendData(indices);
  attachBuffer(ibuff);

  vbuff->bind();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);

  cbuff->bind();
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);

  m_numPoints = 36;
  glBindVertexArray(0);
}
