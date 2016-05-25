#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#include <glm/glm.hpp>
#include <memory>
#include "SceneGraph.hpp"
#include "Util.hpp"
#include "City.hpp"
#include "Doodad.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"
#include "Shader.hpp"

class HeightMap;
class Natural;
class Buildable;
class LandscapeBuilder;
class LandscapeModel;

class LandscapeBuilder
{
  LandscapeBuilder(int seed);
  LandscapeBuilder(const char * ppm);

  void permuteDoodads();
  void permuteCity();
  void permuteLandscape();

  std::shared_ptr<Group> finalize();
private:
  RNG floatGen;
  IntRNG intGen;

  std::shared_ptr<City> city;


  std::shared_ptr<Doodad> genDoodad(int seed);
  std::shared_ptr<City> genCity(int seed);
  std::shared_ptr<LandscapeModel> genLandscapeModel();

  std::vector<HeightMap> genHeightmaps(int seed);
  std::vector<HeightMap> genHeightmaps(int seed, const char * ppm);
};

class HeightMap
{
public:
  virtual float & get(size_t x, size_t y);
  size_t width;
protected:
  std::vector<float> elevations;
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
};

class Buildable : public HeightMap
{
public:
  Buildable(float elevation, size_t size);
  float & get(size_t, size_t) { return elevation; }
private:
  float elevation;
};

class RealData : public HeightMap
{
public:
  RealData(const char * ppm);
};

class LandscapeModel : public Drawable
{
public:
  LandscapeModel(std::vector<float> heights,
                 size_t cols,
                 size_t width);
  void draw();
  std::shared_ptr<Shader> shader;
private:
  GLsizei indices;
  GLuint VAO;
  GLuint VBO;
  GLuint EBO;
};

#endif
