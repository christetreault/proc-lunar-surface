#include "Texture.hpp"

std::map<const char *, std::shared_ptr<Texture> > Texture::memo;

std::shared_ptr<Texture> Texture::getTexture(const char * path)
{
  try
    {
      return memo.at(path);
    }
  catch (std::out_of_range e)
    {
      std::shared_ptr<Texture> curr(new Texture(path));
      memo[path] = curr;
      return curr;
    }
}

Texture::Texture(const char * fp)
{
  std::cerr << "Loading: " << fp << "... ";

  int channels;

  bytes = SOIL_load_image(fp,
                          &width,
                          &height,
                          &channels,
                          SOIL_LOAD_RGB);

  std::cerr << SOIL_last_result() << std::endl;
  assert(bytes != NULL);

  path = fp;
}

GLTexture::GLTexture(const char * fp)
{
  tex = Texture::getTexture(fp);

  glGenTextures(1, &texId);
  glBindTexture(GL_TEXTURE_2D, texId);

  glTexParameteri(GL_TEXTURE_2D,
                  GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGB,
               tex->width,
               tex->height,
               0,
               GL_RGB,
               GL_UNSIGNED_BYTE,
               tex->bytes);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void GLTexture::bind(size_t to)
{
  glActiveTexture(GL_TEXTURE0 + to);
  glBindTexture(GL_TEXTURE_2D, texId);
}

void GLTexture::unbind()
{
  glBindTexture(GL_TEXTURE_2D, 0);
}

CubeMap::CubeMap(const char * right,
          const char * left,
          const char * top,
          const char * bottom,
          const char * front,
          const char * back)
{
  faces[0] = Texture::getTexture(right);
  faces[1] = Texture::getTexture(left);
  faces[2] = Texture::getTexture(top);
  faces[3] = Texture::getTexture(bottom);
  faces[4] = Texture::getTexture(back);
  faces[5] = Texture::getTexture(front);

  glGenTextures(1, &texId);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texId);

  for(GLuint i = 0; i < 6; ++i)
    {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                   0,
                   GL_RGB,
                   faces[i]->width,
                   faces[i]->height,
                   0,
                   GL_RGB,
                   GL_UNSIGNED_BYTE,
                   faces[i]->bytes);
    }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void CubeMap::bind(size_t to)
{
  glActiveTexture(GL_TEXTURE0 + to);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texId);
}

void CubeMap::unbind()
{
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
