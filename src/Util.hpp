#ifndef UTIL_H
#define UTIL_H

#include <glm/glm.hpp>
#include <random>
#include <limits>
#include <ctime>

// maps a value s in the range of sMin-sMax into the range of tMin-tMax
float mapRange(float s, float sMin, float sMax,
                        float tMin, float tMax);

// the trackball mapping function used in pa2-4
glm::vec3 trackBallMapping(float fwidth, float fheight,
                           float xPrime, float yPrime);

// index into a vector as if it were a 2d array
template<typename T>
T & index(std::vector<T> vec, size_t width, size_t x, size_t y)
{
  return vec[(y * width) + x];
}

// index into a vector as if it were a 2d array with a glm::ivec2
template<typename T>
T & index(std::vector<T> vec, size_t width, glm::ivec2 xy)
{
  return index<T>(vec, width, xy.x, xy.y);
}

// index into a vector as if it were a 2d array
template<typename T>
void assign(std::vector<T> & vec, size_t width, size_t x, size_t y, T & val)
{
  vec[(y * width) + x] = val;
}

// index into a vector as if it were a 2d array with a glm::ivec2
template<typename T>
void assign(std::vector<T> & vec, size_t width, glm::ivec2 xy, T & val)
{
  assign<T>(vec, width, xy.x, xy.y, val);
}

class RNG
{
public:
  RNG(unsigned int seed, float min, float max);
  float next();
  unsigned int getSeed() { return seed; }
private:
  unsigned int seed;
  std::mt19937_64 engine;
  std::uniform_real_distribution<float> dist;
};

class IntRNG
{
public:
  IntRNG(unsigned int seed, int min, int max);
  IntRNG(unsigned int seed);
  unsigned int next();
  unsigned int getSeed() { return seed; }
private:
  unsigned int seed;
  std::mt19937_64 engine;
  std::uniform_int_distribution<unsigned int> dist;
};

unsigned int getRandomSeed();

#endif
