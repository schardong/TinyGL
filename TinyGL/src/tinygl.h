#ifndef TINY_GL_H
#define TINY_GL_H

#include "singleton.h"
#include "mesh.h"
#include "shader.h"
#include "light.h"
#include "framebufferobject.h"

#include <string>
#include <map>

enum resource_type
{
  MESH,
  SHADER,
  LIGHT,
  BUFFER,
  FRAMEBUFFER,
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
  void* getResource(resource_type type, std::string name);

  /*bool addMesh(std::string name, Mesh* m);
  bool addShader(std::string name, Shader* s);
  bool addLight(std::string name, Light* l);*/

  void freeResources();
  void draw();

  void draw(std::string name)
  {
    m_meshMap[name]->draw();
  }

  Mesh* getMesh(std::string name)
  {
    return (Mesh*)getResource(MESH, name);;
  }

  Shader* getShader(std::string name)
  {
    return (Shader*)getResource(SHADER, name);
  }

  Light* getLight(std::string name)
  {
    return (Light*)getResource(LIGHT, name);
  }

  BufferObject* getBuffer(std::string name)
  {
    return (BufferObject*)getResource(BUFFER, name);
  }

  FramebufferObject* getFBO(std::string name)
  {
    return (FramebufferObject*)getResource(FRAMEBUFFER, name);
  }

private:
  std::map<std::string, Mesh*> m_meshMap;
  std::map<std::string, Shader*> m_shaderMap;
  std::map<std::string, Light*> m_lightMap;
  std::map<std::string, BufferObject*> m_buffMap;
  std::map<std::string, FramebufferObject*> m_fboMap;
};

#endif // TINY_GL_H
