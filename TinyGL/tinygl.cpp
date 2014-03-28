#include "tinygl.h"
#include "logger.h"
#include "config.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

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

  for (std::map<std::string, ggraf::Shader*>::iterator it = m_shaderMap.begin(); it != m_shaderMap.end(); it++)
    delete it->second;

  m_meshMap.clear();
  m_shaderMap.clear();
}

bool TinyGL::addMesh(std::string name, Mesh* m)
{
  if (m == NULL || name.empty()) return false;
  m_meshMap[name] = new Mesh(*m);
  return true;
}

bool TinyGL::addShader(std::string name, ggraf::Shader* s)
{
  if (s == NULL || name.empty()) return false;
  m_shaderMap[name] = new ggraf::Shader(*s);
  return true;
}