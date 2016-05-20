#ifndef UTIL_H
#define UTIL_H

#include <glm/glm.hpp>

// maps a value s in the range of sMin-sMax into the range of tMin-tMax
float mapRange(float s, float sMin, float sMax,
                        float tMin, float tMax);

// the trackball mapping function used in pa2-4
glm::vec3 trackBallMapping(float fwidth, float fheight,
                           float xPrime, float yPrime);

#endif
