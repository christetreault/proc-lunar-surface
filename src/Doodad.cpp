#include "Doodad.hpp"

static const char * texPath = "res/textures/iridescent.jpg";

// ---------------------------------------------------------
// Doodad --------------------------------------------------
// ---------------------------------------------------------

Doodad::Doodad(float length, float topScale, float bottomScale,
               float topLength, float bottomLength,
               std::shared_ptr<Shader> inShader)
  : model(std::make_shared<Segment>(length, topScale, bottomScale,
                                    topLength, bottomLength, inShader)),
    center(nullptr), topRight(nullptr), topLeft(nullptr),
    bottomRight(nullptr), bottomLeft(nullptr)
{}

void Doodad::insert(std::shared_ptr<Node> what,
                    DoodadMount where,
                    std::function<void(glm::mat4 &, double)> uFn)
{
    switch (where)
    {
    case DoodadMount::center:
      center = std::make_shared<Transform>(what,
                                       model->getMountPoint(where),
                                       uFn);
      break;
    case DoodadMount::topRight:
      topRight = std::make_shared<Transform>(what,
                                       model->getMountPoint(where),
                                       uFn);
      break;
    case DoodadMount::topLeft:
      topLeft = std::make_shared<Transform>(what,
                                       model->getMountPoint(where),
                                       uFn);
      break;
    case DoodadMount::bottomRight:
      bottomRight = std::make_shared<Transform>(what,
                                       model->getMountPoint(where),
                                       uFn);
      break;
    case DoodadMount::bottomLeft:
      bottomLeft = std::make_shared<Transform>(what,
                                       model->getMountPoint(where),
                                       uFn);;
      break;
    default:
      std::cerr << "It should be impossible to reach this case!"
                << std::endl;
      assert(false);
    }
}

void Doodad::getLights(const glm::mat4 & inM,
                         std::vector<std::pair<std::shared_ptr<Light>,
                                               glm::mat4> > & vec) const
{
  if (center != nullptr) center->getLights(inM, vec);
  if (topRight != nullptr) topRight->getLights(inM, vec);
  if (topLeft != nullptr) topLeft->getLights(inM, vec);
  if (bottomRight != nullptr) bottomRight->getLights(inM, vec);
  if (bottomLeft != nullptr) bottomLeft->getLights(inM, vec);
}

void Doodad::getCameras(const glm::mat4 & inM,
                          std::vector<std::pair<std::shared_ptr<Camera>,
                                                glm::mat4> > & vec) const
{
  if (center != nullptr) center->getCameras(inM, vec);
  if (topRight != nullptr) topRight->getCameras(inM, vec);
  if (topLeft != nullptr) topLeft->getCameras(inM, vec);
  if (bottomRight != nullptr) bottomRight->getCameras(inM, vec);
  if (bottomLeft != nullptr) bottomLeft->getCameras(inM, vec);
}

void Doodad::getDrawables(const glm::mat4 & inM,
                            std::vector<std::pair<std::shared_ptr<Drawable>,
                                                  glm::mat4> > & vec) const
{
  if (center != nullptr) center->getDrawables(inM, vec);
  if (topRight != nullptr) topRight->getDrawables(inM, vec);
  if (topLeft != nullptr) topLeft->getDrawables(inM, vec);
  if (bottomRight != nullptr) bottomRight->getDrawables(inM, vec);
  if (bottomLeft != nullptr) bottomLeft->getDrawables(inM, vec);

  auto drawable = std::dynamic_pointer_cast<Drawable>(model);
  vec.push_back(std::make_pair(drawable, inM));
}

void Doodad::update(double time)
{
  if (center != nullptr) center->update(time);
  if (topRight != nullptr) topRight->update(time);
  if (topLeft != nullptr) topLeft->update(time);
  if (bottomRight != nullptr) bottomRight->update(time);
  if (bottomLeft != nullptr) bottomLeft->update(time);
}



// ---------------------------------------------------------
// Segment -------------------------------------------------
// ---------------------------------------------------------

static glm::vec3 genNormal(std::vector<glm::vec3> & verts,
                           size_t center,
                           size_t up,
                           size_t left,
                           size_t down,
                           size_t right)
{
  //std::cerr << "v1: " << glm::to_string(verts[center] - verts[up]) << std::endl;
  //std::cerr << "v1: " << glm::to_string(verts[center] - verts[left]) << std::endl;

  //std::cerr << "v2: " << glm::to_string(verts[center] - verts[left]) << std::endl;
  //std::cerr << "v2: " << glm::to_string(verts[center] - verts[down]) << std::endl;

  //std::cerr << "v3: " << glm::to_string(verts[center] - verts[down]) << std::endl;
  //std::cerr << "v3: " << glm::to_string(verts[center] - verts[right]) << std::endl;

  //std::cerr << "v4: " << glm::to_string(verts[center] - verts[right]) << std::endl;
  //std::cerr << "v4: " << glm::to_string(verts[center] - verts[up]) << std::endl;
  auto n1 = glm::cross(verts[center] - verts[up],
                       verts[center] - verts[left]);
  auto n2 = glm::cross(verts[center] - verts[left],
                       verts[center] - verts[down]);
  auto n3 = glm::cross(verts[center] - verts[down],
                       verts[center] - verts[right]);
  auto n4 = glm::cross(verts[center] - verts[right],
                       verts[center] - verts[up]);

  return glm::normalize((n1 + n2 + n3 + n4) / 4.0f);
}

Segment::Segment(float length, float topScale, float bottomScale,
                 float topLength, float bottomLength,
                 std::shared_ptr<Shader> inShader)
  : shader(inShader),
    tex(texPath, texPath, texPath, texPath, texPath, texPath),
    VAO(0), VBO(0), EBO(0)
{
  std::vector<glm::vec3> verts;
  std::vector<GLuint> idxs;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> texCoords;
  std::vector<DoodadVertex> vertices;

  // vertices

  auto base = 0.0f - bottomLength; // TODO: Clips through the floor.
  auto tip = length + topLength;   // Do I care?
  auto baseCorners = 0.0f;
  auto tipCorners = length;
  auto tipCornersScale = 0.5f * topScale;
  auto baseCornersScale = 0.5f * bottomScale;

  auto top = glm::vec3(0.0f, tip, 0.0f);
  center = top;

  auto topBR = glm::vec3(tipCornersScale, tipCorners, tipCornersScale);
  bottomRight = topBR;
  auto topTR = glm::vec3(tipCornersScale, tipCorners, -tipCornersScale);
  topRight = topTR;
  auto topTL = glm::vec3(-tipCornersScale, tipCorners, -tipCornersScale);
  topLeft = topTL;
  auto topBL = glm::vec3(-tipCornersScale, tipCorners, tipCornersScale);
  bottomLeft = topBL;

  auto botBR = glm::vec3(baseCornersScale, baseCorners, baseCornersScale);
  auto botTR = glm::vec3(baseCornersScale, baseCorners, -baseCornersScale);
  auto botTL = glm::vec3(-baseCornersScale, baseCorners, -baseCornersScale);
  auto botBL = glm::vec3(-baseCornersScale, baseCorners, baseCornersScale);

  auto bot = glm::vec3(0.0f, base, 0.0f);

  verts = { top,   // 0
            topBR, // 1
            topTR, // 2
            topTL, // 3
            topBL, // 4
            botBR, // 5
            botTR, // 6
            botTL, // 7
            botBL, // 8
            bot }; // 9

  // normals

  auto topN = genNormal(verts, 0, 2, 3, 4, 1);
  centerNorm = topN;

  auto topBRN = genNormal(verts, 1, 2, 0, 4, 5);
  bottomRightNorm = topBRN;
  auto topTRN = genNormal(verts, 2, 3, 0, 1, 6);
  topRightNorm = topTRN;
  auto topTLN = genNormal(verts, 3, 4, 0, 2, 7);
  topLeftNorm = topTLN;
  auto topBLN = genNormal(verts, 4, 1, 0, 3, 8);
  bottomLeftNorm = topBLN;

  auto botBRN = genNormal(verts, 5, 1, 8, 9, 6);
  auto botTRN = genNormal(verts, 6, 2, 5, 9, 7);
  auto botTLN = genNormal(verts, 7, 3, 6, 9, 8);
  auto botBLN = genNormal(verts, 8, 4, 7, 9, 5);

  auto botN = genNormal(verts, 9, 5, 8, 7, 6);

  normals = {topN, topBRN, topTRN, topTLN, topBLN,
             botBRN, botTRN, botTLN, botBLN, botN};

  idxs =
    { 0, 1, 2,
      0, 2, 3,
      0, 3, 4,
      0, 4, 1,

      1, 5, 2,
      5, 6, 2,

      2, 6, 3,
      6, 7, 3,

      3, 7, 4,
      7, 8, 4,

      4, 8, 1,
      8, 5, 1,

      5, 9, 6,
      6, 9, 7,
      7, 9, 8,
      8, 9, 5 };

  indices = idxs.size();

  for (size_t count = 0; count < verts.size(); ++count)
    {
      vertices.push_back({ verts[count],
                           normals[count]});

    }

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER,
               vertices.size() * sizeof(DoodadVertex),
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
                        sizeof(DoodadVertex),
                        (GLvoid *) 0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(DoodadVertex),
                        (GLvoid *) offsetof(DoodadVertex, normal));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Segment::draw()
{
  glBindVertexArray(VAO);
  tex.bind(0);
  glUniform1i(glGetUniformLocation(shader->getId(), "tex"), 0);

  glDrawElements(GL_TRIANGLES, (GLsizei) indices, GL_UNSIGNED_INT, 0);

  tex.unbind();
  glBindVertexArray(0);
}

glm::mat4 Segment::getMountPoint(DoodadMount where)
{
  glm::vec3 translate;
  glm::vec3 rotate;

  switch (where)
    {
    case DoodadMount::center:
      translate = center;
      //rotate = centerNorm;
      break;
    case DoodadMount::topRight:
      translate = topRight;
      rotate = topRightNorm;
      break;
    case DoodadMount::topLeft:
      translate = topLeft;
      rotate = topLeftNorm;
      break;
    case DoodadMount::bottomRight:
      translate = bottomRight;
      rotate = bottomRightNorm;
      break;
    case DoodadMount::bottomLeft:
      translate = bottomLeft;
      rotate = bottomLeftNorm;
      break;
    default:
      std::cerr << "It should be impossible to reach this case!"
                << std::endl;
      assert(false);
    }
  // TODO: rotate


  auto trans = glm::translate(glm::mat4(),
                              translate);
  //auto rotY = glm::rotate(trans,
  //                        rotate.y,
  //                        glm::vec3(0.0f, 1.0f, 0.0f));

  auto thetaY = (float) atan2(rotate.x, rotate.z);
  //auto thetaZ = (float) atan2(std::abs(rotate.x), std::abs(rotate.y));
  auto thetaX = (float) atan2(std::abs(rotate.y), std::abs(rotate.z));

  auto rot1 = glm::rotate(glm::mat4(),
                          thetaY,
                          glm::vec3(0.0f, 1.0f, 0.0f));
  auto rot2 = glm::rotate(glm::mat4(),
                          thetaX,
                          glm::vec3(1.0f, 0.0f, 0.0f));
  //auto rot2 = glm::rotate(rotX,
  //                        rotate.z,
  //                        glm::vec3(0.0f, 0.0f, 1.0f));



  return trans * rot1 * rot2;


  // TODO: rotate

  /* Option 2: rotation based on (0 - pos)

  rotate = glm::vec3(0.0, 0.0, 0.0) - translate;

  auto trans = glm::translate(glm::mat4(),
                              translate);
  auto rotY = glm::rotate(trans,
                          rotate.y,
                          glm::vec3(0.0f, 1.0f, 0.0f));
  auto rotX = glm::rotate(rotY,
                          rotate.x,
                          glm::vec3(1.0f, 0.0f, 0.0f));


  return rotX;
  */
}

// ---------------------------------------------------------
// Segment Primitives --------------------------------------
// ---------------------------------------------------------

std::shared_ptr<Doodad> scepter(int seed,
                                float scaleFactor,
                                std::shared_ptr<Shader> shader,
                                std::shared_ptr<Transform> & p1,
                                std::shared_ptr<Transform> & p2,
                                std::shared_ptr<Transform> & p3,
                                std::shared_ptr<Transform> & p4)
{
  IntRNG rng(seed, 1, 4);



  auto base = std::make_shared<Doodad>(4.0f,
                                       1.5f,
                                       0.25f,
                                       0.5f,
                                       0.0f,
                                       shader);
  auto center = std::make_shared<Doodad>(1.0f,
                                         3.0f,
                                         2.5f,
                                         0.5f,
                                         1.0f,
                                         shader);
  auto top = std::make_shared<Doodad>(2.0f,
                                      4.5f,
                                      3.0f,
                                      2.0f,
                                      0.5f,
                                      shader);


  base->insert(center, DoodadMount::center);
  center->insert(top, DoodadMount::center);

  auto scaleVec = glm::vec3(scaleFactor,
                            scaleFactor,
                            scaleFactor);


  p1 = std::make_shared<Transform>(nullptr,
                                   glm::scale(glm::mat4(),
                                              scaleVec));
  center->insert(p1, (DoodadMount) 1);

  p2 = std::make_shared<Transform>(nullptr,
                                   glm::scale(glm::mat4(),
                                              scaleVec));
  center->insert(p2, (DoodadMount) 2);

  p3 = std::make_shared<Transform>(nullptr,
                                   glm::scale(glm::mat4(),
                                              scaleVec));
  center->insert(p3, (DoodadMount) 3);

  p4 = std::make_shared<Transform>(nullptr,
                                   glm::scale(glm::mat4(),
                                              scaleVec));
  center->insert(p4, (DoodadMount) 4);


  return base;
}

std::shared_ptr<Doodad> angryTentacle(int seed,
                                      size_t minSegs,
                                      std::shared_ptr<Shader> shader,
                                      std::shared_ptr<Doodad> & p1)
{
  IntRNG rng(seed, 0, 4);
  auto branches = rng.next() + minSegs;
  size_t curr = 0;
  size_t takenLast = 0;

  std::shared_ptr<Doodad> base;
  std::shared_ptr<Doodad> top;

  base = std::make_shared<Doodad>(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, shader);
  top = base;

  while (curr < branches)
    {
      auto working = std::make_shared<Doodad>(1.0f,
                                              1.0f,
                                              1.0f,
                                              1.0f,
                                              1.0f,
                                              shader);
      auto rolled = rng.next();
      if (rolled == takenLast) continue;
      takenLast = rolled;

      top->insert(working, (DoodadMount) rolled);

      top = working;
      ++curr;
    }
  p1 = top;
  return base;
}

std::shared_ptr<Group> fanout(int seed,
                              float theta,
                              std::shared_ptr<Transform> & p1,
                              std::shared_ptr<Transform> & p2,
                              std::shared_ptr<Transform> & p3)
{
  RNG rng(seed, -0.5f, 0.5f);
  auto fan = std::make_shared<Group>();

  auto rotX1 = glm::rotate(glm::mat4(),
                           theta + rng.next(),
                           glm::vec3(1.0f, 0.0f, 0.0f));
  auto thetaInc = (2.0f * glm::pi<float>()) / 3.0f;

  p1 = std::make_shared<Transform>(nullptr,
                                   rotX1);
  auto rotX2 = glm::rotate(glm::mat4(),
                           theta + rng.next(),
                           glm::vec3(1.0f, 0.0f, 0.0f));
  auto rotY2 = glm::rotate(glm::mat4(),
                           thetaInc,
                           glm::vec3(0.0f, 1.0f, 0.0f)) * rotX2;
  auto rotX3 = glm::rotate(glm::mat4(),
                           theta + rng.next(),
                           glm::vec3(1.0f, 0.0f, 0.0f));
  auto rotY3 = glm::rotate(glm::mat4(),
                           thetaInc * 2.0f,
                           glm::vec3(0.0f, 1.0f, 0.0f)) * rotX3;
  p2 = std::make_shared<Transform>(nullptr, rotY2);
  p3 = std::make_shared<Transform>(nullptr, rotY3);
  fan->insert(p1);
  fan->insert(p2);
  fan->insert(p3);
  return fan;
}

std::shared_ptr<Group> fork(int seed,
                            float theta,
                            std::shared_ptr<Shader> ddShader,
                            std::shared_ptr<Doodad> & l,
                            std::shared_ptr<Doodad> & r)
{
  RNG rng(seed, -0.5f, 0.5f);
  float thetaL = (theta / 2.0f);
  float thetaR = -(theta / 2.0f);

  auto base = std::make_shared<Group>();
  l = std::make_shared<Doodad>(2.0f + rng.next(),
                               1.0f + (rng.next() * 0.5f),
                               1.0f,
                               0.5f + (rng.next() * 0.1f),
                               0.5f,
                               ddShader);
  auto xformL = std::make_shared<Transform>(l,
                                            glm::rotate(glm::mat4(),
                                                        thetaL,
                                                        glm::vec3(1.0f,
                                                                  0.0f,
                                                                  0.0f)));
  r = std::make_shared<Doodad>(2.0f + rng.next(),
                               1.0f + (rng.next() * 0.5f),
                               1.0f,
                               0.5f + (rng.next() * 0.1f),
                               0.5f,
                               ddShader);
  auto xformR = std::make_shared<Transform>(r,
                                            glm::rotate(glm::mat4(),
                                                        thetaR,
                                                        glm::vec3(1.0f,
                                                                  0.0f,
                                                                  0.0f)));
  base->insert(xformL);
  base->insert(xformR);
  return base;
}
