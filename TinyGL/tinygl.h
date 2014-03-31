#ifndef TINY_GL_H
#define TINY_GL_H

#include "singleton.h"
#include "mesh.h"
#include "shader.h"

#include <string>
#include <map>

/**
  TinyGL - A simple manager class that holds the meshes and shaders to be used
  in an OpenGL application. To free the resources just call the method
  "freeResources" declared bellow. TO draw the meshes, the class calls each
  mesh's draw callback, so one must be defined or some error will happen. Since
  this class doesn't  check for errors it will call an invalid method an unpleasant
  things may happen.
 */
class TinyGL : public Singleton<TinyGL>
{
public:
	friend class Singleton<TinyGL>;
  
  bool addMesh(std::string name, Mesh* m);
  bool addShader(std::string name, Shader* s);

  void freeResources();
  void draw();

  void draw(std::string name)
  {
    m_meshMap[name]->draw();
  }

  Shader* getShader(std::string name)
  {
    return m_shaderMap[name];
  }

protected:
  std::map<std::string, Mesh*> m_meshMap;
  std::map<std::string, Shader*> m_shaderMap;

private:
};

#endif // TINY_GL_H