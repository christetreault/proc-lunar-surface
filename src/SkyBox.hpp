#ifndef SKYBOX_H
#define SKYBOX_H

#include <SOIL/SOIL.h>
#include <GL/glew.h>
#include "SceneGraph.hpp"
#include "Shader.hpp"
#include "OBJObject.hpp"
#include "Texture.hpp"

class SkyBox : public Drawable
{
public:
  SkyBox(std::shared_ptr<Shader>,
         GLenum,
         const char *,
         const char *,
         const char *,
         const char *,
         const char *,
         const char *);

  void bind();
  void unbind();
  bool isValid() const;

  ~SkyBox();

  std::shared_ptr<Shader> shader;
  void draw() {model->draw();}
private:

  static SkyBox * curr;
  bool valid;

  std::shared_ptr<OBJObject> model;

  std::shared_ptr<Texture> faces[6];

  GLuint id;
  GLenum texUnit;

  GLuint VAO;
  GLuint VBO;
  GLuint EBO;
};

#endif
