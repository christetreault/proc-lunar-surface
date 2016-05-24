#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#include <glm/glm.hpp>
#include <memory>
#include "SceneGraph.hpp"
#include "Util.hpp"
#include "City.hpp"
#include "Doodad.hpp"
#include "Texture.hpp"

class HeightMap;
class Natural;
class Buildable;
class LandscapeBuilder;
class LandscapeModel;


class Landscape : public Group
{
  Landscape(std::vector<std::pair<glm::mat4,
                                  std::shared_ptr<Doodad>>> doodads,
            std::shared_ptr<City> city,
            std::shared_ptr<LandscapeModel> model);
};

class LandscapeModel : public Drawable
{
public:

private:
  GLuint VAO;
  GLuint VBO;
  GLuint EBO;
};


class LandscapeBuilder
{
  LandscapeBuilder(int seed);
  LandscapeBuilder(const char * ppm);

  void permute(int newSeed);
  std::shared_ptr<Landscape> finalize();
private:
  RNG floatGen;
  IntRNG intGen;

  std::shared_ptr<Doodad> genDoodad();
  std::shared_ptr<City> genCity();
  std::shared_ptr<LandscapeModel> genLandscapeModel();

  std::vector<HeightMap> genHeightmaps();
  std::vector<HeightMap> genHeightmaps(const char * ppm);
};

class HeightMap
{
public:
  virtual float & get(size_t x, size_t y);
  size_t size;
};

class Natural : public HeightMap
{
public:
  Natural(int seed,
          size_t size,
          float topLeft,
          float topRight,
          float bottomLeft,
          float bottomRight);
  virtual float & get(size_t x, size_t y);
private:
  std::vector<float> elevations;
};

class Buildable : public HeightMap
{
public:
  Buildable(float elevation, size_t size);
  virtual float & get(size_t, size_t) { return elevation; }
private:
  float elevation;
};

class RealData : public HeightMap
{
public:
  RealData(const char * ppm);
  virtual float & get(size_t x, size_t y);
private:
  std::vector<float> pixels;
};

#endif
