#ifndef LIGHT_H
#define LIGHT_H

#include "SceneGraph.hpp"
#include <glm/glm.hpp>

class DirLight : public Light
{
public:
  DirLight(const glm::vec3 & inDir, const glm::vec3 & inColor);

  glm::vec3 color;
  glm::vec3 dir;
};


#endif
