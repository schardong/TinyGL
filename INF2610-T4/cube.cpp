#include "cube.h"


Cube::Cube(void)
{
  GLfloat vertices[] = {
    //BACK
    -0.5, -0.5, -0.5,
     0.5, -0.5, -0.5,
     0.5,  0.5, -0.5,
    -0.5, -0.5, -0.5,
     0.5,  0.5, -0.5,
    -0.5,  0.5, -0.5,
    //FRONT
    -0.5, -0.5, 0.5,
     0.5, -0.5, 0.5,
     0.5,  0.5, 0.5,
    -0.5, -0.5, 0.5,
     0.5,  0.5, 0.5,
    -0.5,  0.5, 0.5,
    //RIGHT
     0.5, -0.5,  0.5,
     0.5, -0.5, -0.5,
     0.5,  0.5, -0.5,
     0.5, -0.5,  0.5,
     0.5,  0.5, -0.5,
     0.5,  0.5,  0.5,
     //LEFT
     -0.5, -0.5, -0.5,
     -0.5, -0.5,  0.5,
     -0.5,  0.5,  0.5,
     -0.5, -0.5, -0.5,
     -0.5,  0.5,  0.5,
     -0.5,  0.5, -0.5,
     //TOP
     -0.5,  0.5,  0.5,
      0.5,  0.5,  0.5,
      0.5,  0.5, -0.5,
     -0.5,  0.5,  0.5,
      0.5,  0.5, -0.5,
     -0.5,  0.5, -0.5,
     //BOTTOM
     -0.5, -0.5,  0.5,
      0.5, -0.5,  0.5,
      0.5, -0.5, -0.5,
     -0.5, -0.5,  0.5,
      0.5, -0.5, -0.5,
     -0.5, -0.5, -0.5
  };

  GLfloat normals[] = {
    //BACK
    0, 0, -1,
    0, 0, -1,
    0, 0, -1,
    0, 0, -1,
    0, 0, -1,
    0, 0, -1,
    //FRONT
    0, 0, 1,
    0, 0, 1,
    0, 0, 1,
    0, 0, 1,
    0, 0, 1,
    0, 0, 1,
    //RIGHT
    1, 0, 0,
    1, 0, 0,
    1, 0, 0,
    1, 0, 0,
    1, 0, 0,
    1, 0, 0,
    //LEFT
    -1, 0, 0,
    -1, 0, 0,
    -1, 0, 0,
    -1, 0, 0,
    -1, 0, 0,
    -1, 0, 0,
    //TOP
    0, 1, 0,
    0, 1, 0,
    0, 1, 0,
    0, 1, 0,
    0, 1, 0,
    0, 1, 0,
    //BOTTOM
    0, -1, 0,
    0, -1, 0,
    0, -1, 0,
    0, -1, 0,
    0, -1, 0,
    0, -1, 0
  };
  
  BufferObject* vbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(vertices), GL_STATIC_DRAW);
  vbuff->sendData(&vertices[0]);
  attachBuffer(vbuff);

  BufferObject* nbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(normals), GL_STATIC_DRAW);
  nbuff->sendData(&normals[0]);
  attachBuffer(nbuff);

  bind();

  vbuff->bind();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);

  nbuff->bind();
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  m_numPoints = sizeof(vertices) / sizeof(GLfloat);
}


Cube::~Cube(void)
{
  for (unsigned int i = 0; i < m_buffers.size(); i++)
    delete m_buffers[i];
  m_buffers.clear();

  glDeleteVertexArrays(1, &m_vao);
}
