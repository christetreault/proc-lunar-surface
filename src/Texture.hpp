#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>
#include <map>
#include <iostream>
#include <SOIL/SOIL.h>
#include <GL/glew.h>
#include <cassert>

class Texture
{
private:
  static std::map<const char *, std::shared_ptr<Texture> > memo;
  Texture(const char * fp);

public:
  ~Texture() {SOIL_free_image_data(bytes);}
  static std::shared_ptr<Texture> getTexture(const char * path);

  const char * path;
  unsigned char * bytes;
  int width;
  int height;
};

class GLTexture
{
public:
  GLTexture(const char * fp);

  void bind(size_t to);
  void unbind();

  std::shared_ptr<Texture> tex;
  GLuint texId;
};

class CubeMap
{
public:
  CubeMap(const char *,
          const char *,
          const char *,
          const char *,
          const char *,
          const char *);

  void bind(size_t to);
  void unbind();

  std::shared_ptr<Texture> faces[6];
  GLuint texId;
};

#endif
