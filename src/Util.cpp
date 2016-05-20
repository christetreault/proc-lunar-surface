#include "Util.hpp"

glm::vec3 trackBallMapping(float fwidth, float fheight,
                           float xPrime, float yPrime)
{
  // based on algorithm provided in:
  // http://web.cse.ohio-state.edu/~crawfis/Graphics/VirtualTrackball.html

  glm::vec3 pos;
  float depth;

  pos = { (2.0f * xPrime - fwidth) / fwidth,
          (fheight - 2.0f * yPrime) / fheight,
          0.0f };

  depth = glm::length(pos);

  if (depth > 1.0f) depth = 1.0f;

  pos.z = sqrtf(1.001f - (depth * depth));

  return glm::normalize(pos);
}

float mapRange(float s, float sMin, float sMax,
               float tMin, float tMax)
{
  return tMin + (((s - sMin) * (tMax - tMin)) / (sMax - sMin));
}
