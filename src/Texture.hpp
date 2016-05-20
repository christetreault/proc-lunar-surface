#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>
#include <map>
#include <iostream>
#include <SOIL/SOIL.h>

// This module is icky. I'm open to suggestions

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

#endif
