#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>

// vertex struct used by OBJObject. We'll probably want to enable that texCoord
// filed for this project
typedef struct _Vertex
{
  glm::vec3 position;
  glm::vec3 normal;
  //glm::vec2 texCoord;
} Vertex;

#endif
