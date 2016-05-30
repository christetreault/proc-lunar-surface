#include "Doodad.hpp"

static const char * texPath = "res/textures/iridescent.jpg";
static const char * vertPath = "shader/doodad.vert";
static const char * fragPath = "shader/doodad.frag";

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

Segment::Segment(float length, float topScale, float bottomScale)
  : tex(texPath), VAO(0), VBO(0), EBO(0)
{
  std::vector<glm::vec3> verts;
  std::vector<GLuint> idxs;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> texCoords;
  std::vector<DoodadVertex> vertices;

  // vertices

  auto base = 0.0f;
  auto tip = length;
  auto baseCorners = length / 4.0f;
  auto tipCorners = baseCorners * 3.0f;

  auto top = glm::vec3(0.0f, tip, 0.0f) * topScale;

  auto topBR = glm::vec3(0.5f, tipCorners, 0.5f) * topScale;
  auto topTR = glm::vec3(0.5f, tipCorners, -0.5f) * topScale;
  auto topTL = glm::vec3(-0.5f, tipCorners, -0.5f) * topScale;
  auto topBL = glm::vec3(-0.5f, tipCorners, 0.5f) * topScale;

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

  auto topBRN = genNormal(verts, 1, 2, 0, 4, 5);
  auto topTRN = genNormal(verts, 2, 3, 0, 1, 6);
  auto topTLN = genNormal(verts, 3, 4, 0, 2, 7);
  auto topBLN = genNormal(verts, 4, 1, 0, 3, 8);

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
                           normals[count],
                           glm::vec2(0.0f, 0.0f) });

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

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(DoodadVertex),
                        (GLvoid *) offsetof(DoodadVertex, texCoord));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Shaders

  shader = std::make_shared<Shader>(vertPath, fragPath);
}

void Segment::draw()
{
  glBindVertexArray(VAO);

  glDrawElements(GL_TRIANGLES, (GLsizei) indices, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
}
