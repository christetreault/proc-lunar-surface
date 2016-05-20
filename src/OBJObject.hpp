#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#define GLM_FORCE_RADIANS

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <fstream>
#include <cassert>
#include <boost/tokenizer.hpp>
#include <iterator>
#include <iostream>
#include <GL/glew.h>
#include <glm/gtx/string_cast.hpp>
#include <memory>
#include <map>

#include "Shader.hpp"
#include "Vertex.hpp"
#include "Material.hpp"

class OBJObject
{
private:
  std::vector<GLuint> indices;
  std::vector<Vertex> vertices;

  bool valid;

  void parse(const char* filepath);

  GLuint VAO;
  GLuint VBO;
  GLuint EBO;

  static OBJObject * curr;

  static std::map<const char *, std::shared_ptr<OBJObject> > memo;

  // This was public at one point. I don't think it needs to be.
  // use OBJObject::getObject() to get a pointer to an object.
  // objects are memozied by path, so we don't have 100 bear.obj in
  // memory...
  OBJObject(const char * filepath);
public:

  // Get a shared_ptr to an OBJObject. the object is loaded from
  // file if need be
  static std::shared_ptr<OBJObject> getObject(const char * path);

  ~OBJObject();

  bool isValid() const;
  void bind();
  void unbind();

  void draw();
};

#endif
