#include "tinygl.h"
#include "logger.h"
#include "tglconfig.h"
#include "framebufferobject.h"
#include <GL/glew.h>
#include <string>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

void TinyGL::draw()
{
  for (std::map<std::string, Mesh*>::iterator it = m_meshMap.begin(); it != m_meshMap.end(); it++) {
    it->second->draw();
  }
}

void TinyGL::freeResources()
{
  for (std::map<std::string, Mesh*>::iterator it = m_meshMap.begin(); it != m_meshMap.end(); it++)
    delete it->second;

  for (std::map<std::string, Shader*>::iterator it = m_shaderMap.begin(); it != m_shaderMap.end(); it++)
    delete it->second;

  for (std::map<std::string, Light*>::iterator it = m_lightMap.begin(); it != m_lightMap.end(); it++)
    delete it->second;

  for (std::map<std::string, BufferObject*>::iterator it = m_buffMap.begin(); it != m_buffMap.end(); it++)
    delete it->second;

  for (std::map<std::string, FramebufferObject*>::iterator it = m_fboMap.begin(); it != m_fboMap.end(); it++)
    delete it->second;

  m_meshMap.clear();
  m_shaderMap.clear();
  m_lightMap.clear();
  m_buffMap.clear();
  m_fboMap.clear();
}

bool TinyGL::addResource(resource_type type, std::string name, void* resource)
{
  if (resource == NULL || type > num_resources) return false;
  switch (type) {
  case MESH:
    m_meshMap[name] = new Mesh(*(Mesh*)resource);
    break;
  case SHADER:
    m_shaderMap[name] = new Shader(*(Shader*)resource);
    break;
  case LIGHT:
    m_lightMap[name] = new Light(*(Light*)resource);
    break;
  case BUFFER:
    m_buffMap[name] = new BufferObject(*(BufferObject*)resource);
    break;
  case FRAMEBUFFER:
    m_fboMap[name] = new FramebufferObject(*(FramebufferObject*)resource);
    break;
  }
  return true;
}

void* TinyGL::getResource(resource_type type, std::string name)
{
  if (name.empty() || type > num_resources) return NULL;
  switch (type) {
  case MESH:
    return m_meshMap[name];
    break;
  case SHADER:
    return m_shaderMap[name];
    break;
  case LIGHT:
    return m_lightMap[name];
    break;
  case BUFFER:
    return m_buffMap[name];
    break;
  case FRAMEBUFFER:
    return m_fboMap[name];
    break;
  }
  return NULL;
}

//bool TinyGL::addMesh(std::string name, Mesh* m)
//{
//  if (m == NULL || name.empty()) return false;
//  m_meshMap[name] = new Mesh(*m);
//  return true;
//}
//
//bool TinyGL::addShader(std::string name, Shader* s)
//{
//  if (s == NULL || name.empty()) return false;
//  m_shaderMap[name] = new Shader(*s);
//  return true;
//}
