#include "Light.hpp"

std::shared_ptr<Shader> Light::shadowmap_shader;
DirLight::DirLight(const glm::vec3& inDir, const glm::vec3& inColor) : color(inColor), dir(inDir) {
  if (!shadowmap_shader)
    shadowmap_shader = std::make_shared<Shader>("shader/shadow_map.vert", "shader/shadow_map.frag");
}


