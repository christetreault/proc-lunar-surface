#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#include <glm/glm.hpp>
#include <memory>
#include <algorithm>
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
class City;

class LandscapeBuilder
{
public:
  LandscapeBuilder(int seed);

  void permuteDoodads();
  void permuteCity();
  void permuteLandscape();
  void swapLandscape();

  void finalize();
  std::shared_ptr<Group> landscape;
private:
  IntRNG seedGen;

  std::shared_ptr<City> city;
  std::shared_ptr<LandscapeModel> randomLS;
  std::shared_ptr<LandscapeModel> realDataLS;

  std::shared_ptr<LandscapeModel> currLS;

  std::shared_ptr<Transform> doodad1;
  std::shared_ptr<Transform> doodad2;
  std::shared_ptr<Transform> doodad3;

  std::shared_ptr<Shader> ddShader;
  std::shared_ptr<Shader> lsShader;

  std::shared_ptr<Transform> genDoodad1(int seed);
  std::shared_ptr<Transform> genDoodad2(int seed);
  std::shared_ptr<Transform> genDoodad3(int seed);

  std::shared_ptr<City> genCity();
  std::shared_ptr<LandscapeModel> genLandscapeModel();
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
  HeightMap(unsigned int seed, const char * ppm);
  HeightMap(const char* raw, int);

  size_t width;
  std::vector<float> elevations;
  glm::uvec2 buildSiteCenter;

  std::vector<glm::uvec2> doodads;

  float heightMax;
  float heightMin;
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
                 float heightMin,
                 float heightMax,
                 unsigned int seed,
                 size_t width,
                 glm::uvec2 buildSiteCenter,
                 std::vector<glm::uvec2> doodad,
                 std::shared_ptr<Shader> shader,
                 bool flatten = false);
  void draw();
  std::shared_ptr<Shader> shader;
  glm::vec3 buildSite;

  size_t width;
  std::vector<float> elevations;

  glm::vec3 doodad1;
  glm::vec3 doodad2;
  glm::vec3 doodad3;
private:
  GLTexture stoneTex;
  GLTexture gravelTex;
  GLTexture depositTex;

  GLsizei indices;
  GLuint VAO;
  GLuint VBO;
  GLuint EBO;
};

#endif
