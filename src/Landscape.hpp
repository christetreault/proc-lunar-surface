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
  HeightMap(unsigned int seed,
            size_t n,
            float topLeft,
            float topRight,
            float bottomLeft,
            float bottomRight);
  HeightMap(const char * ppm);

  size_t width;
  std::vector<float> elevations;
  glm::ivec2 buildSiteOffset;
private:
  void diamondSquare(size_t n,
                     float range,
                     unsigned int seed,
                     glm::ivec2 tl,
                     glm::ivec2 tr,
                     glm::ivec2 bl,
                     glm::ivec2 br);

  void safeSquareStep(glm::ivec2 target,
                      glm::ivec2 l,
                      glm::ivec2 u,
                      glm::ivec2 r,
                      glm::ivec2 d,
                      float randVal);
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
  GLTexture stoneTex;
  GLTexture gravelTex;

  GLsizei indices;
  GLuint VAO;
  GLuint VBO;
  GLuint EBO;
};

#endif
