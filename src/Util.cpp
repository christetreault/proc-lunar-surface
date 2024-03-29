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

RNG::RNG(unsigned int inSeed, float min, float max)
  : seed(inSeed), engine(seed), dist(min, max)
{}

float RNG::next()
{
  return dist(engine);
}

IntRNG::IntRNG(unsigned int inSeed, int min, int max)
  : seed(inSeed), engine(seed), dist(min, max)
{}

// suitable for generating a new random seed based on an old one
IntRNG::IntRNG(unsigned int inSeed)
  : seed(inSeed),
    engine(seed),
    dist(std::numeric_limits<unsigned int>::min(),
         std::numeric_limits<unsigned int>::max())
{}

unsigned int IntRNG::next()
{
  return dist(engine);
}

unsigned int getRandomSeed()
{
  unsigned int seed = std::time(nullptr);
  return IntRNG(seed).next();
}

IntSeq::IntSeq(unsigned int inSeed, int min, int max)
  : seed(inSeed), engine(seed), dist(min, max), taken(), min(min), max(max)
{
}

unsigned int IntSeq::next()
{
  // diverges if called too many times. We're too far in the game to
  // concern ourselves with such matters...

  while (true)
    {
      auto curr = dist(engine);
      if (std::find(taken.begin(), taken.end(), curr) == taken.end())
        {
          taken.push_back(curr);
          return curr;
        }
    }
}
