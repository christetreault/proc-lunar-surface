#include "Landscape.hpp"

static const char * vertPath = "shader/heightMap.vert";
static const char * fragPath = "shader/heightMap.frag";

static const char * stonePath = "res/textures/moonStone.jpg";
static const char * gravelPath = "res/textures/gravel.jpg";
static const char * depositPath = "res/textures/iridescent.jpg";

//static const char * terrainPath = "res/terrain/SanDiegoTerrain.tga";
static const char * terrainPath = "res/terrain/moon.tga";

// ---------------------------------------------------------
// LandscapeBuilder ----------------------------------------
// ---------------------------------------------------------

LandscapeBuilder::LandscapeBuilder(int seed, const char * ppm)
  : seedGen(seed), ddShader(std::make_shared<Shader>(ddVertPath, ddFragPath))
{
}

LandscapeBuilder::LandscapeBuilder(int seed)
  : seedGen(seed), ddShader(std::make_shared<Shader>(ddVertPath, ddFragPath))
{}

std::shared_ptr<Group> LandscapeBuilder::genDoodad1(int seed)
{
  auto g = baseOne();
  std::cerr << "g = " << toString(iterate(2,parse(g))) << std::endl;
  auto dd = eval(iterate(2,parse(g)), seed, ddShader);

  auto ddscale = std::make_shared<Transform>(dd,
                                             glm::scale(glm::mat4(),
                                                        glm::vec3(0.005f,
                                                                  0.005f,
                                                                  0.005f)));
  return ddscale;
}

std::shared_ptr<Group> LandscapeBuilder::genDoodad2(int seed)
{
  auto g = baseTwo();
  std::cerr << "g = " << toString(iterate(2,parse(g))) << std::endl;
  auto dd = eval(iterate(2,parse(g)), seed, ddShader);

  auto ddscale = std::make_shared<Transform>(dd,
                                             glm::scale(glm::mat4(),
                                                        glm::vec3(0.005f,
                                                                  0.005f,
                                                                  0.005f)));
  return ddscale;
}

std::shared_ptr<Group> LandscapeBuilder::genDoodad3(int seed)
{
  auto g = baseThree();
  std::cerr << "g = " << toString(iterate(2,parse(g))) << std::endl;
  auto dd = eval(iterate(2,parse(g)), seed, ddShader);

  auto ddscale = std::make_shared<Transform>(dd,
                                             glm::scale(glm::mat4(),
                                                        glm::vec3(0.005f,
                                                                  0.005f,
                                                                  0.005f)));
  return ddscale;
}

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

  auto dd1 = genDoodad1(seedGen.next());
  auto dd1Base = std::make_shared<Transform>(dd1,
                                             glm::translate(glm::mat4(),
                                                            hmModel->doodad1));
  auto dd2 = genDoodad2(seedGen.next());
  auto dd2Base = std::make_shared<Transform>(dd2,
                                             glm::translate(glm::mat4(),
                                                            hmModel->doodad2));

  auto dd3 = genDoodad3(seedGen.next());
  auto dd3Base = std::make_shared<Transform>(dd3,
                                             glm::translate(glm::mat4(),
                                                            hmModel->doodad3));

  scaledRoot->insert(dd1Base);
  scaledRoot->insert(dd2Base);
  scaledRoot->insert(dd3Base);

  // TODO: For testing. Much of this shoul prob move genCity

  //auto ddShader = std::make_shared<Shader>(ddVertPath, ddFragPath);


  //auto g = baseThree();
  //std::cerr << "g = " << toString(iterate(2,parse(g))) << std::endl;
  //auto test = eval(iterate(2,parse(g)), seedGen.next(), ddShader);

  //auto cityScale = std::make_shared<Transform>(test,
  //                                             glm::scale(glm::mat4(),
  //                                                        glm::vec3(0.3f/16.0f,
  //                                                                  0.3f/16.0f,
  //                                                                  0.3f/16.0f)));
  //auto cityBase = std::make_shared<Transform>(cityScale,
  //                                            glm::translate(glm::mat4(),
  //                                                           hmModel->buildSite));
  //scaledRoot->insert(cityBase);

  //root->insert(test);

  return root;
}

std::shared_ptr<LandscapeModel> LandscapeBuilder::genLandscapeModel()
{
  //HeightMap hm(seedGen.next(), terrainPath);;

  size_t n = 6;

  RNG cornerRNG(seedGen.next(), -0.5, 0.5);

  HeightMap hm(seedGen.next(), n,
               cornerRNG.next(),
               cornerRNG.next(),
               cornerRNG.next(),
               cornerRNG.next());

  std::vector<glm::uvec2> ddv = {hm.doodads[0], hm.doodads[1], hm.doodads[2]};

  return std::make_shared<LandscapeModel>(hm.elevations,
                                          hm.heightMin,
                                          hm.heightMax,
                                          seedGen.next(),
                                          hm.width,
                                          hm.buildSiteCenter,
                                          ddv);
}

// ---------------------------------------------------------
// HeightMap -----------------------------------------------
// ---------------------------------------------------------

static std::function<size_t()> sequenceGen(int seed,
                                           int min,
                                           int max)
{
  return [=]() // diverges if used too many times. Solveable, but I have
    {          // other problems...
      static IntRNG rng(seed, min, max);
      static std::vector<size_t> placed;

      while (true)
        {
          auto curr = rng.next();
          if (std::find(placed.begin(), placed.end(), curr) == placed.end())
            {
              placed.push_back(curr);
              return curr;
            }
        }
    };
}

static glm::uvec2 placeZone(size_t xMin, size_t yMin,
                            size_t xMax, size_t yMax, int seed)
{
  IntRNG rngY(seed, yMin, yMax);
  IntRNG rngX(seed, xMin, xMax);
  return glm::uvec2(rngX.next(), rngY.next());
}

HeightMap::HeightMap(unsigned int inSeed,
                     size_t n,
                     float topLeft,
                     float topRight,
                     float bottomLeft,
                     float bottomRight)
{
  IntRNG seedGen(inSeed);
  width = glm::pow(2, n) + 1;
  elevations.resize(width * width);

  heightMin = std::numeric_limits<float>::max();
  heightMax = std::numeric_limits<float>::min();

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

  diamondSquare(n, 0.5f, seedGen.next(), tl, tr, bl, br);

  auto gen = sequenceGen(seedGen.next(), 1, 4);

  auto bsWidth = width / 4;
  auto quadrantWidth = width / 2;

  size_t xMin;
  size_t yMin;

  switch (gen())
    {
    case 1:
      xMin = 0;
      yMin = 0;
      break;
    case 2:
      xMin = quadrantWidth;
      yMin = 0;
      break;
    case 3:
      xMin = 0;
      yMin = quadrantWidth;
      break;
    case 4:
      xMin = quadrantWidth;
      yMin = quadrantWidth;
      break;
    default:
      std::cerr << "gen() generated number out of range!" << std::endl;
      assert(false);
    }

  //IntRNG offsetRNG(seed, 1, (bsWidth * 2) - 1);
  //auto offsetX = offsetRNG.next();
  //auto offsetY = offsetRNG.next();
  auto cityOffset = placeZone(xMin + 1,
                              yMin + 1,
                              xMin + quadrantWidth - bsWidth - 2,
                              yMin + quadrantWidth - bsWidth - 2,
                              seedGen.next());
  auto offsetX = cityOffset.x;
  auto offsetY = cityOffset.y;

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

  // Center doodads

  switch (gen())
    {
    case 1:
      xMin = 0;
      yMin = 0;
      break;
    case 2:
      xMin = quadrantWidth;
      yMin = 0;
      break;
    case 3:
      xMin = 0;
      yMin = quadrantWidth;
      break;
    case 4:
      xMin = quadrantWidth;
      yMin = quadrantWidth;
      break;
    default:
      std::cerr << "gen() generated number out of range!" << std::endl;
      assert(false);
    }

  auto doodadOffset = placeZone(xMin + 1,
                                yMin + 1,
                                xMin + quadrantWidth - bsWidth - 2,
                                yMin + quadrantWidth - bsWidth - 2,
                                seedGen.next());
  offsetX = doodadOffset.x;
  offsetY = doodadOffset.y;

  doodads.push_back( glm::uvec2(offsetX + (bsWidth / 2),
                                offsetY + (bsWidth / 2)));


  switch (gen())
    {
    case 1:
      xMin = 0;
      yMin = 0;
      break;
    case 2:
      xMin = quadrantWidth;
      yMin = 0;
      break;
    case 3:
      xMin = 0;
      yMin = quadrantWidth;
      break;
    case 4:
      xMin = quadrantWidth;
      yMin = quadrantWidth;
      break;
    default:
      std::cerr << "gen() generated number out of range!" << std::endl;
      assert(false);
    }

  doodadOffset = placeZone(xMin + 1,
                           yMin + 1,
                           xMin + quadrantWidth - bsWidth - 2,
                           yMin + quadrantWidth - bsWidth - 2,
                           seedGen.next());
  offsetX = doodadOffset.x;
  offsetY = doodadOffset.y;

  doodads.push_back( glm::uvec2(offsetX + (bsWidth / 2),
                                offsetY + (bsWidth / 2)));


  switch (gen())
    {
    case 1:
      xMin = 0;
      yMin = 0;
      break;
    case 2:
      xMin = quadrantWidth;
      yMin = 0;
      break;
    case 3:
      xMin = 0;
      yMin = quadrantWidth;
      break;
    case 4:
      xMin = quadrantWidth;
      yMin = quadrantWidth;
      break;
    default:
      std::cerr << "gen() generated number out of range!" << std::endl;
      assert(false);
    }


  doodadOffset = placeZone(xMin + 1,
                           yMin + 1,
                           xMin + quadrantWidth - bsWidth - 2,
                           yMin + quadrantWidth - bsWidth - 2,
                           seedGen.next());
  offsetX = doodadOffset.x;
  offsetY = doodadOffset.y;

  doodads.push_back( glm::uvec2(offsetX + (bsWidth / 2),
                                offsetY + (bsWidth / 2)));

}

HeightMap::HeightMap(unsigned int seed, const char * ppm)
{
  IntRNG seedGen(seed);
  int iwidth, iheight, ichannels;
  unsigned char * bytes = SOIL_load_image
    (
     ppm,
     &iwidth, &iheight, &ichannels,
     SOIL_LOAD_L
     );

  heightMin = std::numeric_limits<float>::max();
  heightMax = std::numeric_limits<float>::min();

  for (int x = 0; x < iwidth; ++x)
    {
      for (int y = 0; y < iheight; ++y)
        {
          //std::cerr << "x:y = " << x << ":" << y << std::endl;
          float curr = (float) bytes[y * iwidth + x];
          if (curr > heightMax) heightMax = curr;
          if (curr < heightMin) heightMin = curr;
          elevations.push_back(curr);
          //std::cerr << "curr: " << (mapRange(curr, 0, 255, 0, 1)) << std::endl;
        }
    }



  SOIL_free_image_data(bytes);

  width = iwidth;

  auto gen = sequenceGen(seedGen.next(), 1, 4);

  auto bsWidth = width / 4;
  auto quadrantWidth = width / 2;

  size_t xMin;
  size_t yMin;

  switch (gen())
    {
    case 1:
      xMin = 0;
      yMin = 0;
      break;
    case 2:
      xMin = quadrantWidth;
      yMin = 0;
      break;
    case 3:
      xMin = 0;
      yMin = quadrantWidth;
      break;
    case 4:
      xMin = quadrantWidth;
      yMin = quadrantWidth;
      break;
    default:
      std::cerr << "gen() generated number out of range!" << std::endl;
      assert(false);
    }

  //IntRNG offsetRNG(seed, 1, (bsWidth * 2) - 1);
  //auto offsetX = offsetRNG.next();
  //auto offsetY = offsetRNG.next();
  auto cityOffset = placeZone(xMin + 1,
                              yMin + 1,
                              xMin + quadrantWidth - bsWidth - 2,
                              yMin + quadrantWidth - bsWidth - 2,
                              seedGen.next());
  auto offsetX = cityOffset.x;
  auto offsetY = cityOffset.y;

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

  for (auto & curr : elevations)
    {
      curr = curr - heightMin;
    }
  heightMin = heightMin - heightMin;
  heightMax = heightMax - heightMin;

  // Center doodads

  switch (gen())
    {
    case 1:
      xMin = 0;
      yMin = 0;
      break;
    case 2:
      xMin = quadrantWidth;
      yMin = 0;
      break;
    case 3:
      xMin = 0;
      yMin = quadrantWidth;
      break;
    case 4:
      xMin = quadrantWidth;
      yMin = quadrantWidth;
      break;
    default:
      std::cerr << "gen() generated number out of range!" << std::endl;
      assert(false);
    }


  auto doodadOffset = placeZone(xMin + 1,
                                yMin + 1,
                                xMin + quadrantWidth - bsWidth - 2,
                                yMin + quadrantWidth - bsWidth - 2,
                                seedGen.next());
  offsetX = doodadOffset.x;
  offsetY = doodadOffset.y;

  doodads.push_back( glm::uvec2(offsetX + (bsWidth / 2),
                                offsetY + (bsWidth / 2)));


  switch (gen())
    {
    case 1:
      xMin = 0;
      yMin = 0;
      break;
    case 2:
      xMin = quadrantWidth;
      yMin = 0;
      break;
    case 3:
      xMin = 0;
      yMin = quadrantWidth;
      break;
    case 4:
      xMin = quadrantWidth;
      yMin = quadrantWidth;
      break;
    default:
      std::cerr << "gen() generated number out of range!" << std::endl;
      assert(false);
    }


  doodadOffset = placeZone(xMin + 1,
                           yMin + 1,
                           xMin + quadrantWidth - bsWidth - 2,
                           yMin + quadrantWidth - bsWidth - 2,
                           seedGen.next());
  offsetX = doodadOffset.x;
  offsetY = doodadOffset.y;

  doodads.push_back( glm::uvec2(offsetX + (bsWidth / 2),
                                offsetY + (bsWidth / 2)));


  switch (gen())
    {
    case 1:
      xMin = 0;
      yMin = 0;
      break;
    case 2:
      xMin = quadrantWidth;
      yMin = 0;
      break;
    case 3:
      xMin = 0;
      yMin = quadrantWidth;
      break;
    case 4:
      xMin = quadrantWidth;
      yMin = quadrantWidth;
      break;
    default:
      std::cerr << "gen() generated number out of range!" << std::endl;
      assert(false);
    }


  doodadOffset = placeZone(xMin + 1,
                           yMin + 1,
                           xMin + quadrantWidth - bsWidth - 2,
                           yMin + quadrantWidth - bsWidth - 2,
                           seedGen.next());
  offsetX = doodadOffset.x;
  offsetY = doodadOffset.y;

  doodads.push_back( glm::uvec2(offsetX + (bsWidth / 2),
                                offsetY + (bsWidth / 2)));

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

  if (val > heightMax) heightMax = val;
  if (val < heightMin) heightMin = val;

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

  if (dsRes > heightMax) heightMax = dsRes;
  if (dsRes < heightMin) heightMin = dsRes;

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
                               float heightMin,
                               float heightMax,
                               unsigned int seed,
                               size_t width,
                               glm::uvec2 buildSiteCenter,
                               std::vector<glm::uvec2> doodad)
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
          float yf = mapRange(index<float>(heights, width, x, z) * spacing,
                              heightMin, heightMax, 0.0f, 25.0f);
          //std::cerr << "curr = "
          //          << index<float>(heights, width, x, z)
          //          << " min = " << heightMin
          //          << " max = " << heightMax << std::endl;
          if (yf > maxY) maxY = yf;
          if (yf < minY) minY = yf;
          float zf = (float) z * spacing;
          if (zf > maxZ) maxZ = zf;
          if (zf < minZ) minZ = zf;

          if (!(xf >= 0.0f && xf <= 1.0f))
            {
              std::cerr << "x val out of range: " << xf << std::endl;
            }
          if (!(yf >= 0.0f && yf <= 25.0f))
            {
              std::cerr << "y val out of range: " << yf << std::endl;
              std::cerr << "original y: " << index<float>(heights, width, x, z) * spacing << std::endl;
              std::cerr << "y min/max: " << heightMin << "/" << heightMax << std::endl;
            }
          if (!(zf >= 0.0f && zf <= 1.0f))
            {
              std::cerr << "z val out of range: " << zf << std::endl;
            }

          verts.push_back(glm::vec3(xf, yf, zf));
          if (x == buildSiteCenter.x && z == buildSiteCenter.y)
            {
              buildSite = glm::vec3(xf, yf, zf);
            }
          if (x == doodad[0].x && z == doodad[0].y)
            {
              doodad1 = glm::vec3(xf, yf, zf);
            }
          if (x == doodad[1].x && z == doodad[1].y)
            {
              doodad2 = glm::vec3(xf, yf, zf);
            }
          if (x == doodad[2].x && z == doodad[2].y)
            {
              doodad3 = glm::vec3(xf, yf, zf);
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
  doodad1 = (doodad1 - avg) / largestDiff;
  doodad2 = (doodad2 - avg) / largestDiff;
  doodad3 = (doodad3 - avg) / largestDiff;

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
