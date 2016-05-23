#include "OBJObject.hpp"

std::map<const char *, std::shared_ptr<OBJObject> > OBJObject::memo;

std::shared_ptr<OBJObject> OBJObject::getObject(const char * path)
{
  try
    {
      return memo.at(path);
    }
  catch (std::out_of_range e)
    {
      std::shared_ptr<OBJObject> curr(new OBJObject(path));
      memo[path] = curr;
      return curr;
    }
}

OBJObject::OBJObject(const char * filepath)
{
  valid = false;
  std::cerr << "Loading: " << filepath << "...";
  parse(filepath);
  std::cerr << " done!" << std::endl;

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
               indices.size() * sizeof(GLuint),
               &indices[0],
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

  unbind();

  valid = true;
}

OBJObject::~OBJObject()
{
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}

void OBJObject::parse(const char *filepath)
{
  using namespace std;
  using namespace boost;

  std::vector<glm::vec3> verts;
  std::vector<glm::vec3> norms;

  auto maxX = std::numeric_limits<float>::min();
  auto maxY = std::numeric_limits<float>::min();
  auto maxZ = std::numeric_limits<float>::min();

  auto minX = std::numeric_limits<float>::max();
  auto minY = std::numeric_limits<float>::max();
  auto minZ = std::numeric_limits<float>::max();

  ifstream file(filepath, ios::in);
  string line;
  char_separator<char> sep(" \t\r\n");

  assert(file.is_open());

  while (getline(file, line))
    {
      tokenizer<char_separator<char>> tokens(line, sep);
      auto iter = tokens.begin();

      if (iter == tokens.end()) continue;
      else if (*iter == "v") // ignores trailing optional RGB for now
        {
          ++iter;
          float x = stof(*iter);
          if (x > maxX) maxX = x;
          if (x < minX) minX = x;
          ++iter;
          float y = stof(*iter);
          if (y > maxY) maxY = y;
          if (y < minY) minY = y;
          ++iter;
          float z = stof(*iter);
          if (z > maxZ) maxZ = z;
          if (z < minZ) minZ = z;
          verts.push_back({x, y, z});
        }
      else if (*iter == "vn")
        {
          ++iter;
          float x = stof(*iter);
          ++iter;
          float y = stof(*iter);
          ++iter;
          float z = stof(*iter);
          glm::vec3 working = {x, y, z};
          norms.push_back(glm::normalize(working));
        }
      else if (*iter == "f")
        {
          ++iter;
          indices.push_back((unsigned int) stoul(*iter) - 1);
          ++iter;
          indices.push_back((unsigned int) stoul(*iter) - 1);
          ++iter;
          indices.push_back((unsigned int) stoul(*iter) - 1);
        }
    }
  file.close();

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

  // every vertex must have an associated normal
  assert(verts.size() == norms.size());

  bool inRange = true;

  for (size_t i = 0; i < verts.size(); ++i)
    {
      auto vertPrime = (verts[i] - avg) / largestDiff;
      vertices.push_back({vertPrime, norms[i], glm::vec2(0.0f, 0.0f)});
      if (vertPrime.x > 1.0f || vertPrime.y > 1.0f || vertPrime.z > 1.0f
          || vertPrime.x < -1.0f || vertPrime.y < -1.0f || vertPrime.z < -1.0f)
        {
          std::cerr << "vertex: " << glm::to_string(vertPrime)
                    << " out of range!" << std::endl;
          inRange = false;
        }
    }

  assert(inRange);
}

bool OBJObject::isValid() const
{
  return valid;
}

void OBJObject::bind()
{
  assert (valid);
  glBindVertexArray(VAO);
}

void OBJObject::unbind()
{
  glBindVertexArray(0);
}


void OBJObject::draw()
{
  if (!valid) return;

  glDrawElements(GL_TRIANGLES, (GLsizei) indices.size(), GL_UNSIGNED_INT, 0);

}
