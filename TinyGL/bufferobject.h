#ifndef BUFFEROBJECT_H
#define BUFFEROBJECT_H

#include <GL/glew.h>

/**
 * Class BufferObject
 * This class serves the purpose of abstracting the creation and management of
 * buffer objects of any kind. The user must define the target, size and usage
 * parameters using OpenGL values.
 */
class BufferObject
{
public:
  BufferObject(GLenum target, size_t buff_size, GLenum usage);
  ~BufferObject();

  void allocateStorage(size_t buff_size);
  void sendData(GLvoid* data);

  void bind();
  static void unbind();

  GLuint getId()
  {
    return m_id;
  }

protected:
  GLuint m_id;
  GLenum m_target;
  GLenum m_usage;
  size_t m_size;

  bool m_allocated;
};

#endif // BUFFEROBJECT_H