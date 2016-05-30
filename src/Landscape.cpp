#include "Landscape.hpp"

static const char * vertPath = "shader/heightMap.vert";
static const char * fragPath = "shader/heightMap.frag";

static const char * stonePath = "res/textures/moonStone.jpg";
static const char * gravelPath = "res/textures/gravel.jpg";
static const char * depositPath = "res/textures/iridescent.jpg";

// ---------------------------------------------------------
// LandscapeBuilder ----------------------------------------
// ---------------------------------------------------------

LandscapeBuilder::LandscapeBuilder(int seed)
  : seedGen(seed)
{}

std::shared_ptr<Group> LandscapeBuilder::finalize()
{
  auto root = std::make_shared<Group>();

  auto hmModel = genLandscapeModel();

  auto scaledRoot = std::make_shared<Group>();

  auto baseScale = std::make_shared<Transform>(scaledRoot,
                                               glm::scale(glm::mat4(),
                                                          glm::vec3(16.0f,
                                                                    16.0f,
                                                                    16.0f)));
  scaledRoot->insert(hmModel);
  root->insert(baseScale);

  // TODO: For testing. Much of this shoul prob move genCity
  auto test = std::make_shared<Doodad>(2.0f, 1.0f, 0.5f);
  test->insert(std::make_shared<Doodad>(4.0f, 0.5f, 0.5f),
               DoodadMount::topRight);
  test->insert(std::make_shared<Doodad>(4.0f, 0.5f, 0.5f),
               DoodadMount::bottomLeft);
  test->insert(std::make_shared<Doodad>(4.0f, 0.5f, 0.5f),
               DoodadMount::center);
  auto cityScale = std::make_shared<Transform>(test,
                                               glm::scale(glm::mat4(),
                                                          glm::vec3(0.3f/16.0f,
                                                                    0.3f/16.0f,
                                                                    0.3f/16.0f)));
  auto cityBase = std::make_shared<Transform>(cityScale,
                                              glm::translate(glm::mat4(),
                                                             hmModel->buildSite));
  scaledRoot->insert(cityBase);

  return root;
}

std::shared_ptr<LandscapeModel> LandscapeBuilder::genLandscapeModel()
{
  size_t n = 6;

  RNG cornerRNG(seedGen.next(), -0.5, 0.5);

  HeightMap hm(seedGen.next(), n,
               cornerRNG.next(),
               cornerRNG.next(),
               cornerRNG.next(),
               cornerRNG.next());

  return std::make_shared<LandscapeModel>(hm.elevations,
                                          seedGen.next(),
                                          hm.width,
                                          hm.buildSiteCenter);
}

// ---------------------------------------------------------
// HeightMap -----------------------------------------------
// ---------------------------------------------------------

HeightMap::HeightMap(unsigned int seed,
                     size_t n,
                     float topLeft,
                     float topRight,
                     float bottomLeft,
                     float bottomRight)
{
  width = glm::pow(2, n) + 1;
  elevations.resize(width * width);

  for (size_t count = 0; count < (width * width); ++count)
    {
      elevations[count] = 0.0f;
    }

  for (size_t count = 0; count < (width * width); ++count)
    {
      assert(elevations[count] == 0.0f);
    }
  auto tl = glm::ivec2(0, 0);
  auto tr = glm::ivec2(width - 1, 0);
  auto bl = glm::ivec2(0, width - 1);
  auto br = glm::ivec2(width - 1, width - 1);

  assign<float>(elevations, width, tl, topLeft);
  assign<float>(elevations, width, tr, topRight);
  assign<float>(elevations, width, bl, bottomLeft);
  assign<float>(elevations, width, br, bottomRight);

  diamondSquare(n, 0.5f, seed, tl, tr, bl, br);

  auto bsWidth = width / 4;
  IntRNG offsetRNG(seed, 1, (bsWidth * 2) - 1);
  auto offsetX = offsetRNG.next();
  auto offsetY = offsetRNG.next();

  buildSiteCenter = glm::uvec2(offsetX + (bsWidth / 2),
                               offsetY + (bsWidth / 2));

  auto average = (index<float>(elevations, width, offsetX, offsetY)
                  + index<float>(elevations, width, offsetX + bsWidth, offsetY)
                  + index<float>(elevations, width, offsetX, offsetY + bsWidth)
                  + index<float>(elevations, width,
                                 offsetX + bsWidth, offsetY + bsWidth))
    / 4.0f;

  for (size_t x = offsetX; x <= offsetX + bsWidth; ++x)
    {
      for (size_t y = offsetY; y <= offsetY + bsWidth; ++y)
        {
          assign<float>(elevations, width, x, y, average);
        }
    }
}

void HeightMap::safeSquareStep(glm::ivec2 target,
                               glm::ivec2 l,
                               glm::ivec2 u,
                               glm::ivec2 r,
                               glm::ivec2 d,
                               float randVal)
{
  float sum = 0.0f;
  float denom = 0.0f;

  if (l.x >= 0 && ((size_t) l.x) < width && l.y >= 0 && ((size_t) l.y) < width)
    {
      sum = sum + index<float>(elevations, width,
                               (size_t) l.x, (size_t) l.y);
      denom = denom + 1.0f;
    }
  if (u.x >= 0 && ((size_t) u.x) < width && u.y >= 0 && ((size_t) u.y) < width)
    {
      sum = sum + index<float>(elevations, width,
                               (size_t) u.x, (size_t) u.y);
      denom = denom + 1.0f;
    }
  if (r.x >= 0 && ((size_t) r.x) < width && r.y >= 0 && ((size_t) r.y) < width)
    {
      sum = sum + index<float>(elevations, width,
                               (size_t) r.x, (size_t) r.y);
      denom = denom + 1.0f;
    }
  if (d.x >= 0 && ((size_t) d.x) < width && d.y >= 0 && ((size_t) d.y) < width)
    {
      sum = sum + index<float>(elevations, width,
                               (size_t) d.x, (size_t) d.y);
      denom = denom + 1.0f;
    }

  assert(denom > 0.0f);

  float val = (sum / denom) + randVal;
  // if (!(val > -20.0f && val < 20.0f))
  //   {
  //     std::cerr << "l = " << glm::to_string(l)
  //               << "u = " << glm::to_string(u)
  //               << "r = " << glm::to_string(r)
  //               << "d = " << glm::to_string(d)
  //               << std::endl;
  //     std::cerr << "sum = " << sum
  //               << " denom = " << denom
  //               << " randVal = " << randVal << std::endl;
  //     std::cerr << "val = " << val
  //               << " recalculated = " << (sum / denom) + randVal <<std::endl;
  //     assert(false);
  //   }
  assign<float>(elevations, width, target, val);
}

void HeightMap::diamondSquare(size_t n,
                              float range,
                              unsigned int seed,
                              glm::ivec2 tl,
                              glm::ivec2 tr,
                              glm::ivec2 bl,
                              glm::ivec2 br)
{
  // sanity check that this thing is a square
  assert(tl.x == bl.x);
  assert(tr.x == br.x);
  assert(tl.y == tr.y);
  assert(bl.y == br.y);

  if (n == 0) return; // nNext would be -1

  // spin up relevant values

  size_t nNext = n - 1;
  size_t widthNext = glm::pow(2, nNext);

  RNG rng(seed, range * -1.0f, range);
  IntRNG seedGen(seed);

  float rangeNext = (range / 2.0f) - (0.1f * glm::abs(rng.next()));

  auto tc = glm::ivec2(tl.x + widthNext, tl.y);
  auto lc = glm::ivec2(tl.x, tl.y + widthNext);
  auto rc = glm::ivec2(tr.x, tr.y + widthNext);
  auto bc = glm::ivec2(tl.x + widthNext, bl.y);

  // These can potentiall be out of range
  auto dtc = glm::ivec2(tc.x, tc.y - widthNext);
  auto dlc = glm::ivec2(lc.x - widthNext, lc.y);
  auto drc = glm::ivec2(rc.x + widthNext, rc.y);
  auto dbc = glm::ivec2(bc.x, bc.y + widthNext);

  auto center = glm::ivec2(tl.x + widthNext,
                           tl.y + widthNext);
  // diamond step

  float randVal = rng.next();
  float dsRes = ((index<float>(elevations, width, tl) +
                  index<float>(elevations, width, tr) +
                  index<float>(elevations, width, bl) +
                  index<float>(elevations, width, br)) / 4.0f) + randVal;


  assert(tl.x >= 0 && ((size_t) tl.x) < width && tl.y >= 0 && ((size_t) tl.y) < width);
  assert(tr.x >= 0 && ((size_t) tr.x) < width && tr.y >= 0 && ((size_t) tr.y) < width);
  assert(bl.x >= 0 && ((size_t) bl.x) < width && bl.y >= 0 && ((size_t) bl.y) < width);
  assert(br.x >= 0 && ((size_t) br.x) < width && br.y >= 0 && ((size_t) br.y) < width);
  if (!(dsRes > -1000.0f && dsRes < 1000.0f))
    {
      std::cerr << "dsRes = " << dsRes << std::endl;
      std::cerr << "tl = " << glm::to_string(tl) << std::endl;
      std::cerr << "tlVal = " << index<float>(elevations, width, tl) << std::endl;
      std::cerr << "tr = " << glm::to_string(tr) << std::endl;
      std::cerr << "trVal = " << index<float>(elevations, width, tr) << std::endl;
      std::cerr << "bl = " << glm::to_string(bl) << std::endl;
      std::cerr << "blVal = " << index<float>(elevations, width, bl) << std::endl;
      std::cerr << "br = " << glm::to_string(br) << std::endl;
      std::cerr << "brVal = " << index<float>(elevations, width, br) << std::endl;
      std::cerr << "randVal = " << randVal << std::endl;
      assert(false);
    }
  assign<float>(elevations, width, center, dsRes);

  // square step

  safeSquareStep(tc,
                 dtc,
                 glm::ivec2(tl),
                 glm::ivec2(tr),
                 glm::ivec2(center),
                 rng.next());
  safeSquareStep(lc,
                 dlc,
                 glm::ivec2(tl),
                 glm::ivec2(bl),
                 glm::ivec2(center),
                 rng.next());
  safeSquareStep(rc,
                 drc,
                 glm::ivec2(tr),
                 glm::ivec2(br),
                 glm::ivec2(center),
                 rng.next());
  safeSquareStep(bc,
                 dbc,
                 glm::ivec2(bl),
                 glm::ivec2(br),
                 glm::ivec2(center),
                 rng.next());

  // recurse

  diamondSquare(nNext,
                rangeNext,
                seedGen.next(),
                tl,
                tc,
                lc,
                center);
  diamondSquare(nNext,
                rangeNext,
                seedGen.next(),
                tc,
                tr,
                center,
                rc);
  diamondSquare(nNext,
                rangeNext,
                seedGen.next(),
                lc,
                center,
                bl,
                bc);
  diamondSquare(nNext,
                rangeNext,
                seedGen.next(),
                center,
                rc,
                bc,
                br);
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

static void placeDeposits(std::vector<size_t> & deps,
                          unsigned int seed,
                          size_t size,
                          size_t amt)
{
  IntRNG rng(seed, 0, size - 1);
  size_t placed = 0;

  while (placed < amt)
    {
      auto curr = rng.next();
      if (std::find(deps.begin(), deps.end(), curr) == deps.end())
        {
          deps.push_back(curr);
          ++placed;
        }
    }
}

LandscapeModel::LandscapeModel(std::vector<float> heights,
                               unsigned int seed,
                               size_t width,
                               glm::uvec2 buildSiteCenter)
  : stoneTex(stonePath), gravelTex(gravelPath),
    depositTex(depositPath), VAO(0), VBO(0), EBO(0)
{
  std::vector<glm::vec3> verts;
  std::vector<GLuint> idxs;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> texCoords;
  std::vector<LandscapeVertex> vertices;

  std::vector<size_t> deps;
  placeDeposits(deps, seed, heights.size(), heights.size() / 40);

  // Vertices

  auto maxX = std::numeric_limits<float>::min();
  auto maxY = std::numeric_limits<float>::min();
  auto maxZ = std::numeric_limits<float>::min();

  auto minX = std::numeric_limits<float>::max();
  auto minY = std::numeric_limits<float>::max();
  auto minZ = std::numeric_limits<float>::max();

  assert(width > 1); // Don't waste our time

  auto spacing = 1.0f / (((float) width - 1));

  for (size_t z = 0; z < width; ++z)
    {
      for (size_t x = 0; x < width; ++x)
        {
          float xf = (float) x * spacing;
          if (xf > maxX) maxX = xf;
          if (xf < minX) minX = xf;
          float yf = index<float>(heights, width, x, z);
          if (yf > maxY) maxY = yf;
          if (yf < minY) minY = yf;
          float zf = (float) z * spacing;
          if (zf > maxZ) maxZ = zf;
          if (zf < minZ) minZ = zf;

          verts.push_back(glm::vec3(xf, yf, zf));
          if (x == buildSiteCenter.x && z == buildSiteCenter.y)
            {
              buildSite = glm::vec3(xf, yf, zf);
            }
        }
    }

  auto largestDiff = 1.0f;
  auto diffX = maxX - minX;
  auto diffY = maxY - minY;
  auto diffZ = maxZ - minZ;
  glm::vec3 avg =
    {
      (maxX + minX) / 2,
      (maxY + minY) / 2,
      (maxZ + minZ) / 2
    };

  if (diffX > largestDiff) largestDiff = diffX;
  if (diffY > largestDiff) largestDiff = diffY;
  if (diffZ > largestDiff) largestDiff = diffZ;

  for (auto & curr : verts)
    {
      curr = (curr - avg) / largestDiff;
      texCoords.push_back(glm::vec2(curr.x, curr.z));
    }

  buildSite = (buildSite - avg) / largestDiff;

  // Indices

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
          idxs.push_back((width - 1) + ((y + 1) * width));
        }
    }

  indices = idxs.size();

  // Normals

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

  // OpenGL boilerplate

  for (size_t count = 0; count < verts.size(); ++count)
    {
      float isDep = (std::find(deps.begin(),
                               deps.end(),
                               count) != deps.end()) ? 1.0f : 0.0f;

      vertices.push_back({verts[count],
                          normals[count],
                          texCoords[count],
                          isDep});
    }

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER,
               vertices.size() * sizeof(LandscapeVertex),
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
                        sizeof(LandscapeVertex),
                        (GLvoid *) 0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(LandscapeVertex),
                        (GLvoid *) offsetof(LandscapeVertex, normal));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(LandscapeVertex),
                        (GLvoid *) offsetof(LandscapeVertex, texCoord));

  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 1,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(LandscapeVertex),
                        (GLvoid *) offsetof(LandscapeVertex, isDeposit));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Shaders

  shader = std::make_shared<Shader>(vertPath, fragPath);
}

void LandscapeModel::draw()
{
  glBindVertexArray(VAO);
  stoneTex.bind(0);
  glUniform1i(glGetUniformLocation(shader->getId(), "stoneTex"), 0);
  gravelTex.bind(1);
  glUniform1i(glGetUniformLocation(shader->getId(), "gravelTex"), 1);
  depositTex.bind(2);
  glUniform1i(glGetUniformLocation(shader->getId(), "depositTex"), 2);
  glDisable(GL_CULL_FACE);
  glDrawElements(GL_TRIANGLE_STRIP, (GLsizei) indices, GL_UNSIGNED_INT, 0);
  glEnable(GL_CULL_FACE);
  gravelTex.unbind();
  stoneTex.unbind();
  depositTex.unbind();
  //glDrawElements(GL_POINTS, (GLsizei) indices, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
