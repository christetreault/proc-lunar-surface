#ifndef DOODAD_H
#define DOODAD_H

#include <set>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <boost/tokenizer.hpp>
#include <sstream>
#include <iomanip>
#include "SceneGraph.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "Util.hpp"

static const char * ddVertPath = "shader/doodad.vert";
static const char * ddFragPath = "shader/doodad.frag";

enum class DoodadMount
{
  center = 0, topRight = 1, topLeft = 2, bottomRight = 3, bottomLeft = 4
};

typedef struct _SegmentKey
{
  int length;
  int topScale;
  int bottomScale;
  int topLength;
  int bottomLength;
} SegmentKey;

inline bool operator==(const SegmentKey & lhs,
                       const SegmentKey & rhs)
{
  return (lhs.length == rhs.length)
    && (lhs.topScale == rhs.topScale)
    && (lhs.bottomScale == rhs.bottomScale)
    && (lhs.topLength == rhs.topLength)
    && (lhs.bottomLength == rhs.bottomLength);
}

inline bool operator<(const SegmentKey & lhs,
                      const SegmentKey & rhs)
{
  if (lhs == rhs) return false;

  if (lhs.length < rhs.length)
    {
      //std::cerr << "lhs.length < rhs.length"<<lhs.length <<"<"<< rhs.length<<std::endl;
      return true;
    }
  else if (lhs.topScale < rhs.topScale)
    {
      //std::cerr <<"lhs.topScale < rhs.topScale"<< lhs.topScale <<"<"<< rhs.topScale<<std::endl;
      return true;
    }
  else if (lhs.bottomScale < rhs.bottomScale)
    {
      //std::cerr << "lhs.bottomScale == rhs.bottomScale"<<lhs.bottomScale <<"<"<< rhs.bottomScale<<std::endl;
      return true;
    }
  else if (lhs.topLength < rhs.topLength)
    {
      //std::cerr << "lhs.topLength == rhs.topLength"<<lhs.topLength <<"<"<< rhs.topLength<<std::endl;
      return true;
    }
  else if (lhs.bottomLength < rhs.bottomLength)
    {
      //std::cerr << "lhs.bottomLength == rhs.bottomLength"<<lhs.bottomLength <<"<"<< rhs.bottomLength<<std::endl;
      return true;
    }
  else return false;
}

class Segment : public Drawable
{
public:
  void draw();
  static std::shared_ptr<Segment> getSegment(float length, float topScale,
                                             float bottomScale,
                                             float topLength,
                                             float bottomLength,
                                             std::shared_ptr<Shader> inShader);
  std::shared_ptr<Shader> shader;
  glm::mat4 getMountPoint(DoodadMount where);
  static void clearMemo()
  {
    std::cerr << "memo size: " << memo.size() << std::endl;
    memo = std::map<std::string, std::shared_ptr<Segment> >();
    std::cerr << "memo size: " << memo.size() << std::endl;
  }
private:
  Segment(float length, float topScale, float bottomScale,
          float topLength, float bottomLength,
          std::shared_ptr<Shader> inShader);
  static std::map<std::string, std::shared_ptr<Segment> > memo;
  CubeMap tex;
  GLuint VAO;
  GLuint VBO;
  GLuint EBO;

  size_t indices;

  glm::vec3 center;
  glm::vec3 topLeft;
  glm::vec3 topRight;
  glm::vec3 bottomLeft;
  glm::vec3 bottomRight;

  glm::vec3 centerNorm;
  glm::vec3 topLeftNorm;
  glm::vec3 topRightNorm;
  glm::vec3 bottomLeftNorm;
  glm::vec3 bottomRightNorm;
};

class Doodad : public Group
{ // a Doodad is a thing that can be generated by L-systems
public:

  Doodad(float length, float topScale, float bottomScale,
         float topLength, float bottomLength,
         std::shared_ptr<Shader> inShader);

  virtual void insert(std::shared_ptr<Node> what)
  {insert(what, DoodadMount::center);}

  virtual void insert(std::shared_ptr<Node> what,
                      DoodadMount where,
                      std::function<void(glm::mat4 &, double)> uFn = doNothing);

  virtual void getLights(const glm::mat4 & inM,
                         std::vector<std::pair<std::shared_ptr<Light>,
                                               glm::mat4> > & vec) const;

  virtual void getCameras(const glm::mat4 & inM,
                          std::vector<std::pair<std::shared_ptr<Camera>,
                                                glm::mat4> > & vec) const;

  virtual void getDrawables(const glm::mat4 & inM,
                            std::vector<std::pair<std::shared_ptr<Drawable>,
                                                  glm::mat4> > & vec) const;

  virtual void update(double time);

private:
  std::shared_ptr<Segment> model;
  std::shared_ptr<Transform> center;
  std::shared_ptr<Transform> topRight;
  std::shared_ptr<Transform> topLeft;
  std::shared_ptr<Transform> bottomRight;
  std::shared_ptr<Transform> bottomLeft;

};

typedef struct _Grammar
{
  char ctor;
  std::vector<float> args;
  std::shared_ptr<struct _Grammar> lhs;
  std::shared_ptr<struct _Grammar> rhs;
} Grammar;

std::shared_ptr<Grammar> parse(std::string str);
std::string toString(std::shared_ptr<Grammar> g);
std::shared_ptr<Grammar> iterate(size_t n, std::shared_ptr<Grammar> g);
std::shared_ptr<Group> eval(std::shared_ptr<Grammar> g,
                            int seed,
                            std::shared_ptr<Shader> shader);

/*
  V = { D(len, topScale, bottomScale, topLen, bottomLen), -- Doodad
        C(len, topScale, bottomScale, topLen, bottomLen), -- Doodad(center only)
        S(scale), -- scepter
        T(minSegs) } -- angryTentacle
  S = { F(theta), -- fanout in 3 directions
        K(theta), -- fork in 2 directions
        A(r, s) -- for all directions produced by r, do subrule s }
  ω = { }
  ω = { }
  ω = { }

 */



// D(len, topScale, bottomScale, topLen, bottomLen)
//    -> A(D(len, topScale, bottomScale, topLen, bottomLen), ?)

// S(scale) -> A(S(scale), ?)
std::shared_ptr<Transform> scepter(int seed,
                                   float scaleFactor,
                                   std::shared_ptr<Shader> shader,
                                   std::shared_ptr<Transform> & p1,
                                   std::shared_ptr<Transform> & p2,
                                   std::shared_ptr<Transform> & p3,
                                   std::shared_ptr<Transform> & p4);

// T(minSegs) -> A(T(minSegs), ?)
std::shared_ptr<Doodad> angryTentacle(int seed,
                                      size_t minSegs,
                                      std::shared_ptr<Shader> shader,
                                      std::shared_ptr<Doodad> & p1);

// F(theta, r1, r2, r3)
std::shared_ptr<Group> fanout(int seed,
                              float theta,
                              std::shared_ptr<Transform> & p1,
                              std::shared_ptr<Transform> & p2,
                              std::shared_ptr<Transform> & p3);
// K(theta, r1, r2, r3)
std::shared_ptr<Group> fork(int seed,
                            float theta,
                            //std::shared_ptr<Shader> ddShader,
                            std::shared_ptr<Transform> & l,
                            std::shared_ptr<Transform> & r);

std::string baseOne();
std::string baseTwo();
std::string baseThree();
#endif
