#include "Doodad.hpp"

static const char * texPath = "res/textures/iridescent.jpg";

// ---------------------------------------------------------
// Doodad --------------------------------------------------
// ---------------------------------------------------------

Doodad::Doodad(float length, float topScale, float bottomScale,
         std::shared_ptr<Shader> inShader)
  : model(std::make_shared<Segment>(length, topScale, bottomScale, inShader)),
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

  auto base = (-length) / 4.0f;      // TODO: Clips through the floor.
  auto tip = (3.0f * length) / 4.0f; // Do I care?
  auto baseCorners = 0.0f;
  auto tipCorners = length / 2.0f;

  auto top = glm::vec3(0.0f, tip, 0.0f) * topScale;
  center = top;

  auto topBR = glm::vec3(0.5f, tipCorners, 0.5f) * topScale;
  bottomRight = topBR;
  auto topTR = glm::vec3(0.5f, tipCorners, -0.5f) * topScale;
  topRight = topTR;
  auto topTL = glm::vec3(-0.5f, tipCorners, -0.5f) * topScale;
  topLeft = topTL;
  auto topBL = glm::vec3(-0.5f, tipCorners, 0.5f) * topScale;
  bottomLeft = topBL;

  auto botBR = glm::vec3(0.5f, baseCorners, 0.5f) * bottomScale;
  auto botTR = glm::vec3(0.5f, baseCorners, -0.5f) * bottomScale;
  auto botTL = glm::vec3(-0.5f, baseCorners, -0.5f) * bottomScale;
  auto botBL = glm::vec3(-0.5f, baseCorners, 0.5f) * bottomScale;

  auto bot = glm::vec3(0.0f, base, 0.0f) * bottomScale;

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
      rotate = centerNorm;
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
  auto rotY = glm::rotate(trans,
                          rotate.y,
                          glm::vec3(0.0f, 1.0f, 0.0f));
  auto rotX = glm::rotate(rotY,
                          -rotate.x,
                          glm::vec3(1.0f, 0.0f, 0.0f));
  auto rotZ = glm::rotate(rotX,
                          rotate.z,
                          glm::vec3(0.0f, 0.0f, 1.0f));

  return rotZ;

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
