#include "Light.hpp"


DirLight::DirLight(const glm::vec3& inDir, const glm::vec3& inColor)
  : color(inColor), dir(inDir) {}
