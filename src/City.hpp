#ifndef CITY_H
#define CITY_H


#include "SceneGraph.hpp"
#include "Util.hpp" // RNG
#include "Landscape.hpp"

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class LandscapeModel;

class RoadNetwork : public Drawable {
public:
  std::vector<float> roads;
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> quad_vertices;
  std::shared_ptr<LandscapeModel> landscape;
  static std::shared_ptr<Shader> shader;

  RoadNetwork(std::vector<float> roads, std::shared_ptr<LandscapeModel> landscape);

  void draw() {
    glBindVertexArray(VAO);
    float t = glfwGetTime();
    int n = t / 5.0 * quad_vertices.size();
    if (n > quad_vertices.size()) n = quad_vertices.size();
    glDrawArrays(GL_QUADS, 0, n);
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
