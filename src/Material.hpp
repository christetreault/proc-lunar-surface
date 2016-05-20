#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>

class Material
{
public:
  Material(const glm::vec3 & inAmbient,
           const glm::vec3 & inDiffuse,
           const glm::vec3 & inSpecular,
           float inShininess)
    : ambient(inAmbient),
      diffuse(inDiffuse),
      specular(inSpecular),
      shininess(inShininess) {}

  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  float shininess;
};

#endif
