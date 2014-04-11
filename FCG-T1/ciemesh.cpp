#include "ciemesh.h"

CIEMesh::CIEMesh(std::vector<glm::vec3> xyz, size_t slices)
{
  std::vector<GLfloat> vertices(xyz.size() * 3);
  
  for(size_t i = 0; i < vertices.size(); i += 3) {
    vertices[i] = xyz[i/3].x;
    vertices[i + 1] = xyz[i/3].y;
    vertices[i + 2] = xyz[i/3].z;
  }
  
  std::vector<GLfloat> colors(vertices.size());
  for(size_t i = 0; i < colors.size(); i += 3) {
    colors[i] = vertices[i];
    colors[i + 1] = vertices[i + 1];
    colors[i + 2] = vertices[i + 2];
  }

  std::vector<GLushort> indices;

  //Connect the (0,0,0) point with the first row of points. These points
  //are at the X + Y + Z = slices plane.
  size_t tmp_addr = (xyz.size() - 1) / slices;
  for (size_t i = 1; i < tmp_addr; i++) {
    indices.push_back(i);
    indices.push_back(i + 1);
    indices.push_back(0);
  }

  //Connect the middle points. Form the triangles from the i row with the
  //i + 1 row.
  for (size_t i = 1; i < (slices - 1) * tmp_addr - 1; i++) {
    indices.push_back(i);
    indices.push_back(i + tmp_addr);
    indices.push_back(i+1);

    indices.push_back(i + tmp_addr);
    indices.push_back(i + tmp_addr + 1);
    indices.push_back(i + 1);
  }

  //Connect all the points with the white point (which is suposed to be the
  //last point on the set).
  tmp_addr *= (slices - 1);
  size_t max_x = 0;
  for (size_t i = tmp_addr; i < xyz.size() - 2; i++) {
    indices.push_back(i);
    indices.push_back(xyz.size() - 1);
    indices.push_back(i + 1);
    if (xyz[i].x > xyz[max_x].x)
      max_x = i;
  }

  //Connect the black, blue and red point. This creates the purple line and
  //The triangle that is under the spectrum.
  indices.push_back(tmp_addr + 1);
  indices.push_back(0);
  indices.push_back(max_x);

  //Connect the white, blue and red point. This closes the spectrum by creating
  //the triangle at it's front.
  indices.push_back(tmp_addr + 1);
  indices.push_back(max_x);
  indices.push_back(xyz.size() - 1);
  
  BufferObject* vbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), GL_STATIC_DRAW);
  vbuff->sendData(&vertices[0]);
  attachBuffer(vbuff);

  BufferObject* cbuff = new BufferObject(GL_ARRAY_BUFFER, sizeof(GLfloat) * colors.size(), GL_STATIC_DRAW);
  cbuff->sendData(&colors[0]);
  attachBuffer(cbuff);

  bind();

  BufferObject* ibuff = new BufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indices.size(), GL_STATIC_DRAW);
  ibuff->sendData(&indices[0]);
  attachBuffer(ibuff);

  vbuff->bind();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);

  cbuff->bind();
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);
  
  m_numPoints = indices.size();

  vertices.clear();
  colors.clear();
  indices.clear();
}

CIEMesh::~CIEMesh()
{
}
