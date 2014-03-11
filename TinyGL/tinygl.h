#ifndef TINY_GL_H
#define TINY_GL_H

#include "singleton.h"
#include "mesh.h"
#include "shader.h"
#include <iup.h>
#include <string>
#include <map>

using std::string;

class TinyGL : public Singleton<TinyGL>
{
public:
	friend class Singleton<TinyGL>;
  
  bool addMesh(string name, Mesh* m);
  bool addShader(string name, ggraf::Shader* s);

  void freeResources();
  void draw();

  void draw(string name)
  {
    m_meshMap[name]->draw();
  }

  ggraf::Shader* getShader(string name)
  {
    return m_shaderMap[name];
  }

protected:
  std::map<string, Mesh*> m_meshMap;
  std::map<string, ggraf::Shader*> m_shaderMap;

private:
};

#endif // TINY_GL_H