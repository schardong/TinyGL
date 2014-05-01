#include "tinygl.h"
#include "logger.h"
#include "tglconfig.h"
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

  m_meshMap.clear();
  m_shaderMap.clear();
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
  }
  return true;
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
