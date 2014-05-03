#ifndef TINY_GL_H
#define TINY_GL_H

#include "singleton.h"
#include "mesh.h"
#include "shader.h"
#include "light.h"

#include <string>
#include <map>

enum resource_type
{
  MESH,
  SHADER,
  LIGHT,
  num_resources
};

/**
 * class TinyGL
 * A simple manager class that holds the meshes and shaders to be used
 * in an OpenGL application. To free the resources just call the method
 * "freeResources" declared bellow. To draw the meshes, the class calls each
 * mesh's draw callback, so one must be defined or some error will happen. Since
 * this class doesn't  check for errors it will call an invalid method an unpleasant
 * things may happen.
 * The meshes and shaders are stored as maps on this class. They may be added and
 * retrived by their names. These resources are all destroyed when the freeResources
 * method is called, so make copies if you wish to keep them after calling this method.
 */
class TinyGL : public Singleton<TinyGL>
{
public:
	friend class Singleton<TinyGL>;
  
  bool addResource(resource_type type, std::string name, void* resource);

  /*bool addMesh(std::string name, Mesh* m);
  bool addShader(std::string name, Shader* s);
  bool addLight(std::string name, Light* l);*/

  void freeResources();
  void draw();

  void draw(std::string name)
  {
    m_meshMap[name]->draw();
  }

  void* getResource(resource_type type, std::string name)
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
    }
    return NULL;
  }

  Shader* getShader(std::string name)
  {
    return (Shader*)getResource(SHADER, name);
  }

  Mesh* getMesh(std::string name)
  {
    return (Mesh*)getResource(MESH, name);;
  }

private:
  std::map<std::string, Mesh*> m_meshMap;
  std::map<std::string, Shader*> m_shaderMap;
  std::map<std::string, Light*> m_lightMap;
};

#endif // TINY_GL_H
