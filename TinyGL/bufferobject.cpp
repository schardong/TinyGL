#include "bufferobject.h"
#include <iostream>

BufferObject::BufferObject()
{
  glGenBuffers(1, &m_id);
}

BufferObject::BufferObject(GLenum target, size_t buff_size, GLenum usage) :
  m_target(target), m_size(0), m_allocated(false), m_usage(usage)
{
  glGenBuffers(1, &m_id);
  bind();
  allocateStorage(buff_size);
}

BufferObject::~BufferObject()
{
  glDeleteBuffers(1, &m_id);
  m_allocated = false;
}

void BufferObject::allocateStorage(size_t buff_size)
{
  m_size = buff_size;
  glBufferData(m_target, m_size, NULL, m_usage);
  m_allocated = true;
}

void BufferObject::sendData(GLvoid* data)
{
  if (!m_allocated) {
    allocateStorage(m_size);
    m_allocated = true;
  }
  glBufferSubData(m_target, 0, m_size, data);
}

void BufferObject::bind()
{
  glBindBuffer(m_target, m_id);
}

void BufferObject::unbind()
{
  GLenum targets = GL_ARRAY_BUFFER | GL_ELEMENT_ARRAY_BUFFER | GL_COPY_READ_BUFFER | GL_COPY_WRITE_BUFFER | GL_DRAW_INDIRECT_BUFFER
    | GL_PIXEL_PACK_BUFFER | GL_PIXEL_UNPACK_BUFFER | GL_TEXTURE_BUFFER | GL_TRANSFORM_FEEDBACK_BUFFER | GL_UNIFORM_BUFFER;

  glBindBuffer(targets, 0);
}