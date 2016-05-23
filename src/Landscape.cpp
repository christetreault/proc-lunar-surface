#include "Landscape.hpp"

// ---------------------------------------------------------
// LandscapeGrid -------------------------------------------
// ---------------------------------------------------------

LandscapeGrid::LandscapeGrid(unsigned int seed)
  : rng(seed, -1.0f, 1.0f)
{
  //TODO : create 9 tiles
}

float & LandscapeGrid::get(size_t x, size_t y)
{
  // TODO: math
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

void LandscapePatch::regenerate(std::function<float&(size_t,size_t)> get,
                                size_t size)
{
  std::vector<glm::vec3> verts;
  std::vector<GLuint> idxs;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> texCoords;

  auto spacing = 1.0f / ((float) size);

  for (size_t y = 0; y < size; ++y)
    {
      for (size_t x = 0; x < size; ++x)
        {
          verts.push_back(glm::vec3(x * spacing,
                                    y * spacing,
                                    get(x, y)));
        }
    }

  // based on example at: http://www.learnopengles.com/tag/height-maps/
  for (GLuint y = 0; y < size; ++y)
    {
      if (y > 0) idxs.push_back(y * size);
      for (GLuint x = 0; x < size; ++x)
        {
          idxs.push_back((y * size) + x);
          idxs.push_back(((y + 1) * size) + x);
        }
      if (y < size - 1)
        {
          idxs.push_back(((y + 1) * size) + (size - 1));
        }
    }

}

LandscapePatch::LandscapePatch()
  : valid(false), VAO(0), VBO(0), EBO(0)
{}
