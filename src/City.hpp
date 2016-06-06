#ifndef CITY_H
#define CITY_H

#include "SceneGraph.hpp"
#include "Util.hpp" // RNG
#include "Landscape.hpp"

class LandscapeModel;

class RoadNetwork : public Drawable {
public:
  std::vector<float> roads;
  std::vector<glm::vec3> vertices;
  std::shared_ptr<LandscapeModel> landscape;
  static std::shared_ptr<Shader> shader;

  RoadNetwork(std::vector<float> roads, std::shared_ptr<LandscapeModel> landscape);

  void draw() {
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, vertices.size());
    glBindVertexArray(0);
  }
private:
  GLuint VAO, VBO;

};

class City : public Group /* or whatever else sort of Node suits you */
{
public:
  City(unsigned int seed/* TODO: other params */);

  std::shared_ptr<LandscapeModel> ground;
  std::shared_ptr<RoadNetwork> roads;
private:
};

#endif
