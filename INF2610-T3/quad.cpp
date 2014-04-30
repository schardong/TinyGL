#include "quad.h"

Quad::Quad()
{
  GLfloat vertices[] = {
    0, 0, 0,
    1, 0, 0,
    1, 1, 0,
    0, 1, 0
  };

  GLfloat texCoord[] = {
    0, 0,
    1, 0,
    1, 1,
    0, 1
  };
    
  GLubyte indices[] = {
    0, 1, 2,
    0, 2, 3
  };
  

  BufferObject* vbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat) * 12, GL_STATIC_DRAW);
  vbuff->sendData(&vertices[0]);
  attachBuffer(vbuff);

  BufferObject* tbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, GL_STATIC_DRAW);
  tbuff->sendData(&texCoord[0]);
  attachBuffer(tbuff);

  bind();

  BufferObject* ibuff = new BufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 6, GL_STATIC_DRAW);
  ibuff->sendData(&indices[0]);
  attachBuffer(ibuff);

  vbuff->bind();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);

  tbuff->bind();
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  m_numPoints = 6;
}

Quad::~Quad()
{
  for (unsigned int i = 0; i < m_buffers.size(); i++)
    delete m_buffers[i];
  m_buffers.clear();

  glDeleteVertexArrays(1, &m_vao);
}
