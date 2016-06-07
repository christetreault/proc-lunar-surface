#include <Python.h>
#include <stdlib.h>
#include "City.hpp"

using namespace std;
using namespace glm;

static const char* groundVertPath = "shader/heightMap.vert";
static const char* groundFragPath = "shader/heightMap.frag";

extern int main_argc;
extern char** main_argv;

shared_ptr<Shader> RoadNetwork::shader;
vector<float> read_floats(int len, string filename) {
  float buf[len];
  FILE* f = fopen(filename.c_str(), "r");
  if (!f) {
    std::cerr << "Failed to open " << filename << std::endl;
    assert(false);
  }
  len = fread(buf, 4, len, f);
  fclose(f);
  return std::vector<float>(buf, buf + len);
}

City::City(unsigned int seed) {
//  Py_Initialize();
//  PySys_SetArgvEx(main_argc, main_argv, 1);
  //  FILE* f = fopen("citygen/export.py", "r");
  //    PyRun_SimpleFile(f, "citygen/export.py");
  //  fclose(f);
//    cout << "Generating city" << endl;
//    PyRun_SimpleString("import os, sys\nprint os.getcwd()\nprint sys.path\nimport citygen.export\n");
//  Py_Finalize();

  // Welp
   system("python2 citygen/export.py");
  int width = 512;

  vector<float> elevations = read_floats(width * width, "res/terrain/citygen_heightmap");
  ground = make_shared<LandscapeModel>(elevations, 0, 0.25, 0, width, uvec2(), vector<uvec2>(),
                                       std::make_shared<Shader>(groundVertPath, groundFragPath));

  insert(ground);
  roads = std::make_shared<RoadNetwork>(read_floats(width * width, "res/terrain/citygen_roads"),
                                        ground);
  insert(roads);
}

RoadNetwork::RoadNetwork(std::vector<float> roads, std::shared_ptr<LandscapeModel> landscape)
        : roads(roads), landscape(landscape) {
  if (!shader) {
    shader = std::make_shared<Shader>("shader/road.vert", "shader/road.frag");
  }


  cout << roads.size() << endl;
  auto i = roads.begin();
  while (i != roads.end()) {
    glm::vec3 v;
    v.x = *i++;
    v.z = *i++;

    v.y = landscape->elevations[((int) (v.z)) * landscape->width + ((int) (v.x))];

    v.y += 0.001;
    v.x *= (1.0 / landscape->width);
    v.z *= (1.0 / landscape->width);

    v.x -= 0.5;
    v.z -= 0.5;
    vertices.push_back(v);
  }

  auto j = vertices.begin();
  while (j != vertices.end()) {
    bool use_last = false;
//    if (j != vertices.begin() and *j == *(j - 1)) use_last = true;
    vec3 a = *j++;
    vec3 b = *j++;
    vec3 forward = normalize(b - a);
    vec3 right = normalize(cross(forward, vec3(0, 1, 0)));
    if (use_last) {
      cout << "Using last" << endl;
      quad_vertices.push_back(*(quad_vertices.end() - 1));
      quad_vertices.push_back(*(quad_vertices.end() - 3));
    }
    else {
      quad_vertices.push_back(a - right * 0.002f);
      quad_vertices.push_back(a + right * 0.002f);
    }
    quad_vertices.push_back(b + right * 0.002f);
    quad_vertices.push_back(b - right * 0.002f);

  }

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, quad_vertices.size() * sizeof(float) * 3, &quad_vertices[0],
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (GLvoid*) 0);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindVertexArray(0);
}
