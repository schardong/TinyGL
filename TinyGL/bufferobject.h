#ifndef BUFFEROBJECT_H
#define BUFFEROBJECT_H

#include <GL/glew.h>

class BufferObject
{
public:
  BufferObject();
  BufferObject(GLenum target, size_t buff_size, GLenum usage);
  ~BufferObject();

  void allocateStorage(size_t buff_size);
  void sendData(GLvoid* data);

  void bind();
  static void unbind();

protected:
  GLuint m_id;
  GLenum m_target;
  GLenum m_usage;
  size_t m_size;

  bool m_allocated;
};

#endif // BUFFEROBJECT_H