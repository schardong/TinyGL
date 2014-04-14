#include "srgbmesh.h"
#include "fcgt1config.h"
#include <cstring>

extern "C" {
#include "color.h"
}

GLfloat cube_vertices[] = {
  0, 0, 0,
  1, 0, 0,
  1, 1, 0,
  0, 1, 0,
  0, 0, 1,
  1, 0, 1,
  1, 1, 1,
  0, 1, 1
};

GLbyte cube_indices[] {
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

glm::mat3 srgbToXYZ = { 0.412, 0.357, 0.180, 0.212, 0.715, 0.072, 0.019, 0.119, 0.950 };

SRGBMesh::SRGBMesh()
{
  GLfloat colors[24];
  memcpy(colors, cube_vertices, sizeof(GLfloat) * 24);

  BufferObject* vbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24, GL_STATIC_DRAW);
  vbuff->sendData(cube_vertices);
  attachBuffer(vbuff);

  BufferObject* cbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24, GL_STATIC_DRAW);
  cbuff->sendData(colors);
  attachBuffer(cbuff);

  bind();

  BufferObject* ibuff = new BufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 36, GL_STATIC_DRAW);
  ibuff->sendData(cube_indices);
  attachBuffer(ibuff);

  vbuff->bind();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);

  cbuff->bind();
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);

  m_numPoints = 36;
  glBindVertexArray(0);

  m_colorspace = colorspace::sRGB;
}

void SRGBMesh::convertColorspace()
{
  switch (m_colorspace) {
  case colorspace::CIEXYZ:
    m_modelMatrix = glm::mat4(srgbToXYZ);
    buildGeometry();
    break;
  case colorspace::CIERGB:
    break;
  case colorspace::CIELab:
    break;
  case colorspace::sRGB:
    buildGeometry();
    break;
  }
}

void SRGBMesh::buildGeometry()
{
  switch (m_colorspace) {
  case colorspace::sRGB:
  case colorspace::CIEXYZ:
    m_buffers[0]->sendData(cube_vertices);
    bind();
    m_buffers[2]->sendData(cube_indices);
    break;
  case colorspace::CIERGB:
    break;
  case colorspace::CIELab:
    break;
  }
}