#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#include <glm/glm.hpp>
#include "SceneGraph.hpp"
#include "Util.hpp"
#include "City.hpp"
#include "Doodad.hpp"

class Landscape;
class Natural;
class Buildable;
class LandscapePatch;

class LandscapeGrid : public Group
{
public:
  LandscapeGrid(unsigned int seed);
private:
  RNG rng;
};

class Landscape : public Group
{
public:
  virtual float & get(size_t x, size_t y) = 0;
  size_t size;
};

class Natural : public Landscape
{
public:
  Natural(RNG & rng,
          size_t size,
          float topLeft,
          float topRight,
          float bottomLeft,
          float bottomRight);
  virtual float & get(size_t x, size_t y);
private:
  std::vector<float> elevations;
};

class Buildable : public Landscape
{
public:
  Buildable(float elevation, size_t size, RNG & rng);

  float & get(size_t, size_t) { return elevation; }

  City city;

private:
    float elevation;
};

class LandscapePatch : public Drawable
{
  LandscapePatch();
  void regenerate(std::shared_ptr<Landscape> target);
  bool isValid() { return valid; }
private:
  bool valid;
  GLuint VAO;
  GLuint VBO;
  GLuint EBO;
};

#endif
