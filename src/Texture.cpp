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
  std::cerr << "Loading: " << fp << "...";

  int channels;

  bytes = SOIL_load_image(fp,
                          &width,
                          &height,
                          &channels,
                          SOIL_LOAD_RGB);
  std::cerr << " done!" << std::endl;
  path = fp;
}
