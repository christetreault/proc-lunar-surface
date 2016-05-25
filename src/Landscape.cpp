#include "Landscape.hpp"

static const char * vertPath = "shader/heightMap.vert";
static const char * fragPath = "shader/heightMap.frag";

// ---------------------------------------------------------
// LandscapeBuilder ----------------------------------------
// ---------------------------------------------------------



// ---------------------------------------------------------
// HeightMap -----------------------------------------------
// ---------------------------------------------------------

float & HeightMap::get(size_t x, size_t y)
{
  return index<float>(elevations, width, x, y);
}

// ---------------------------------------------------------
// Natural -------------------------------------------------
// ---------------------------------------------------------

Natural::Natural(int seed,
                 size_t size,
                 float topLeft,
                 float topRight,
                 float bottomLeft,
                 float bottomRight)
{
  elevations.resize(size * size);
  width = size;
  for (size_t count = 0; count < size; ++count)
    {
      elevations[count] = 0.0f;
    }
  get(0, 0) = topLeft;
  get(size - 1, 0) = topRight;
  get(0, size - 1) = bottomLeft;
  get(size - 1, size - 1) = bottomRight;
}

// ---------------------------------------------------------
// Buildable -----------------------------------------------
// ---------------------------------------------------------

Buildable::Buildable(float inElevation, size_t inSize)
{
  elevation = inElevation;
  width = inSize;
}

// ---------------------------------------------------------
// RealData ------------------------------------------------
// ---------------------------------------------------------

RealData::RealData(const char * ppm)
{
  // TODO: read ppm
}

// ---------------------------------------------------------
// LandscapeModel ------------------------------------------
// ---------------------------------------------------------

void crossIdxAcc(std::vector<glm::vec3> & verts,
                 size_t width,
                 size_t x1,
                 size_t y1,
                 size_t x2,
                 size_t y2,
                 size_t x3,
                 size_t y3,
                 glm::vec3 & acc,
                 size_t & additions)
{
  acc = acc + glm::cross(index<glm::vec3>(verts, width, x1, y1)
                         - index<glm::vec3>(verts, width, x2, y2),
                         index<glm::vec3>(verts, width, x1, y1)
                         - index<glm::vec3>(verts, width, x3, y3));
  ++additions;
}

LandscapeModel::LandscapeModel(std::vector<float> heights,
                               size_t cols,
                               size_t width)
  : VAO(0), VBO(0), EBO(0)
{
  std::vector<glm::vec3> verts;
  std::vector<GLuint> idxs;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> texCoords;
  std::vector<Vertex> vertices;

  assert(width > 1); // Don't waste our time

  auto spacing = ((float) cols) / (((float) width - 1));

  for (size_t y = 0; y < width; ++y)
    {
      for (size_t x = 0; x < width; ++x)
        {
          verts.push_back(glm::vec3(x * spacing,
                                    y * spacing,
                                    index<float>(heights, width, x, y)));
        }
    }

  // based on example at: http://www.learnopengles.com/tag/height-maps/
  for (GLuint y = 0; y < width - 1; ++y)
    {
      if (y > 0) idxs.push_back(y * width);
      for (GLuint x = 0; x < width; ++x)
        {
          idxs.push_back(x + (y * width));
          idxs.push_back(x + ((y + 1) * width));
        }
      if (y < (width - 1))
        {
          std::cerr << "y = " << y
                    << "width = " << width
                    << std::endl;
          idxs.push_back((width - 1) + ((y + 1) * width));
        }
    }

  for (const auto & curr : idxs)
    {
      std::cerr << glm::to_string(curr) << std::endl;
    }
  std::cerr << "|verts| = " << verts.size() << std::endl;

  indices = idxs.size();

  for (size_t y = 0; y < width; ++y)
    {
      for (size_t x = 0; x < width; ++x)
        {
          size_t adds = 0;
          auto sum = glm::vec3(0.0f);

          if (y == 0 && x == 0)
            {
              crossIdxAcc(verts, width, x, y, x,  y + 1, x + 1, y, sum, adds);
            }
          else if (y == 0 && x == (width - 1))
            {
              crossIdxAcc(verts, width, x, y, x - 1, y, x - 1, y + 1, sum, adds);
              crossIdxAcc(verts, width, x, y, x - 1, y + 1, x, y + 1, sum, adds);
            }
          else if (y == (width - 1) && x == 0)
            {
              crossIdxAcc(verts, width, x, y, x + 1, y, x + 1, y - 1, sum, adds);
              crossIdxAcc(verts, width, x, y, x + 1, y - 1, x, y - 1, sum, adds);
            }
          else if (y == (width - 1) && x == (width - 1))
            {
              crossIdxAcc(verts, width, x, y, x, y - 1, x - 1, y, sum, adds);
            }
          else if (y == 0)
            {
              crossIdxAcc(verts, width, x, y, x - 1, y, x - 1, y + 1, sum, adds);
              crossIdxAcc(verts, width, x, y, x - 1, y + 1, x, y + 1, sum, adds);
              crossIdxAcc(verts, width, x, y, x, y + 1, x + 1, y, sum, adds);
            }
          else if (y == (width - 1))
            {
              crossIdxAcc(verts, width, x, y, x, y - 1, x - 1, y, sum, adds);
              crossIdxAcc(verts, width, x, y, x + 1, y, x + 1, y - 1, sum, adds);
              crossIdxAcc(verts, width, x, y, x + 1, y - 1, x, y - 1, sum, adds);
            }
          else if (x == 0)
            {
              crossIdxAcc(verts, width, x, y, x, y + 1, x + 1, y, sum, adds);
              crossIdxAcc(verts, width, x, y, x + 1, y, x + 1, y - 1, sum, adds);
              crossIdxAcc(verts, width, x, y, x + 1, y - 1, x, y - 1, sum, adds);
            }
          else if (x == (width - 1))
            {
              crossIdxAcc(verts, width, x, y, x, y - 1, x - 1, y, sum, adds);
              crossIdxAcc(verts, width, x, y, x - 1, y, x - 1, y + 1, sum, adds);
              crossIdxAcc(verts, width, x, y, x - 1, y + 1, x, y + 1, sum, adds);
            }
          else // interior vertex
            {
              crossIdxAcc(verts, width, x, y, x, y - 1, x - 1, y, sum, adds);
              crossIdxAcc(verts, width, x, y, x - 1, y, x - 1, y + 1, sum, adds);
              crossIdxAcc(verts, width, x, y, x - 1, y + 1, x, y + 1, sum, adds);
              crossIdxAcc(verts, width, x, y, x, y + 1, x + 1, y, sum, adds);
              crossIdxAcc(verts, width, x, y, x + 1, y, x + 1, y - 1, sum, adds);
              crossIdxAcc(verts, width, x, y, x + 1, y - 1, x, y - 1, sum, adds);
            }

          normals.push_back(glm::normalize(sum / ((float) adds)));
        }
    }

  // TODO: texture coordinates

  for (size_t count = 0; count < verts.size(); ++count)
    {
      vertices.push_back({verts[count],
                          normals[count],
                          glm::vec2(0.0f, 0.0f)});
    }

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER,
               vertices.size() * sizeof(Vertex),
               &vertices[0],
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               idxs.size() * sizeof(GLuint),
               &idxs[0],
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        (GLvoid *) 0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        (GLvoid *) offsetof(Vertex, normal));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        (GLvoid *) offsetof(Vertex, texCoord));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  shader = std::make_shared<Shader>(vertPath, fragPath);
}

void LandscapeModel::draw()
{
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLE_STRIP, (GLsizei) indices, GL_UNSIGNED_INT, 0);
  //glDrawElements(GL_POINTS, (GLsizei) indices, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
