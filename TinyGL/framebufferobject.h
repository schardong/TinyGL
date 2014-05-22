#ifndef FRAMEBUFFEROBJECT_H
#define FRAMEBUFFEROBJECT_H

#include <map>
#include <GL/glew.h>

class FramebufferObject
{
public:
  FramebufferObject();
  ~FramebufferObject();

  void attachTexBuffer(GLenum target, GLenum attach, GLenum textarg, GLuint texid, GLint level);
  void attachRenderBuffer(GLenum target, GLenum attach, GLenum rbtarg, GLuint rbid); 

  void bind(GLenum target)
  {
    if(!isBound())
      glBindFramebuffer(target, m_id);
  }

  static void unbind()
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  inline bool isBound()
  {
    return m_bound;
  }

  void checkStatus();

private:
  GLuint m_id;
  GLuint m_width;
  GLuint m_height;
  bool m_bound;

  std::map<GLenum, GLuint> m_attMap;
};

#endif // FRAMEBUFFEROBJECT_H