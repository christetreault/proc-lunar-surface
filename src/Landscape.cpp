#include "Landscape.hpp"

// ---------------------------------------------------------
// LandscapeGrid -------------------------------------------
// ---------------------------------------------------------

LandscapeGrid::LandscapeGrid(unsigned int seed)
  : rng(seed, -1.0f, 1.0f)
{
  //TODO : create 9 tiles
}

// ---------------------------------------------------------
// Landscape -----------------------------------------------
// ---------------------------------------------------------

// ---------------------------------------------------------
// Natural -------------------------------------------------
// ---------------------------------------------------------

Natural::Natural(RNG & rng,
                 size_t size,
                 float topLeft,
                 float topRight,
                 float bottomLeft,
                 float bottomRight)
{
  elevations.resize(size * size);
  for (size_t count = 0; count < size; ++count)
    {
      elevations[count] = 0.0f;
    }
  get(0, 0) = topLeft;
  get(size - 1, 0) = topRight;
  get(0, size - 1) = bottomLeft;
  get(size - 1, size - 1) = bottomRight;

  // TODO: spin up some doodads
}

float & Natural::get(size_t x, size_t y)
{
  // TODO: math
}

// ---------------------------------------------------------
// Buildable -----------------------------------------------
// ---------------------------------------------------------

Buildable::Buildable(float inElevation, size_t size, RNG & rng)
  : city(rng.getSeed())
{
  elevation = inElevation;
}

// ---------------------------------------------------------
// LandscapePatch ------------------------------------------
// ---------------------------------------------------------

void LandscapePatch::regenerate(std::shared_ptr<Landscape> target)
{
  std::vector<glm::vec3> verts;
  std::vector<GLuint> idxs;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> texCoords;
}

LandscapePatch::LandscapePatch()
  : valid(false), VAO(0), VBO(0), EBO(0)
{}
