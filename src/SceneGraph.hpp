#ifndef SCENEGRAPH_H
#define SCENEGRAPH_H

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "OBJObject.hpp"
#include "Material.hpp"
#include "Shader.hpp"

enum class LightType;

auto doNothing = [](glm::mat4 &, double){};

class Node
{
public:
  virtual ~Node() {};

  // am I a [whatever]? prevents excessive use of typeid()
  virtual bool isLight() const {return false;}
  virtual bool isCamera() const {return false;}
  virtual bool isDrawable() const {return false;}
  virtual bool isGroup() const {return false;}
  virtual bool isTransform() const {return false;}
};

// base class of all light types
class Light : public Node
{
public:
  bool isLight() const {return true;}
};

// base class of all camera types
class Camera : public Node
{
public:
  // Get the View matrix (P in PVM) given an M matrix for this camera
  virtual glm::mat4 getV(const glm::mat4 & M) const = 0;

  // Get the position of this camera given an M matrix
  virtual glm::vec3 getPos(const glm::mat4 & M) const = 0;
};

// A camera that orbits around (0, 0, 0) used in PA3 and PA4
class OrbitalCamera : public Camera
{
public:
  OrbitalCamera(glm::vec3 inUp, glm::vec3 inPos)
    : up(inUp) , pos(inPos), orbit(glm::mat4()),
      move(glm::mat4()) {}
  bool isCamera() const {return true;}
  glm::vec3 up;
  glm::vec3 pos;
  glm::mat4 orbit;
  glm::mat4 move;
  virtual glm::mat4 getV(const glm::mat4 & M) const;
  virtual glm::vec3 getPos(const glm::mat4 & M) const
  { return glm::vec3(M * orbit * move * glm::vec4(pos, 1.0f)); }
  void reset() {orbit = glm::mat4(); move = glm::mat4();}
};

// base class of things that can be drawn
class Drawable : public Node
{
public:
  virtual bool isDrawable() const {return true;}
  virtual void draw() = 0;
};

class OBJDrawable : public Drawable
{
public:
  OBJDrawable(std::shared_ptr<OBJObject> model,
              std::shared_ptr<Shader> shader,
              std::shared_ptr<Material> material);

  std::shared_ptr<OBJObject> model;
  std::shared_ptr<Shader> shader;
  std::shared_ptr<Material> material;

  void draw() {model->draw();}
};

// groupings of nodes. Does not apply transformations
class Group : public Node
{
public:
  Group() {}

  virtual void insert(std::shared_ptr<Node> what) {children.push_back(what);}
  //virtual void remove(std::shared_ptr<Node> what);
  // virtual void clear() {children.clear();}

  bool isGroup() const {return true;}

  // gets all child lights, paired with their M matrix
  virtual void getLights(const glm::mat4 & inM,
                         std::vector<std::pair<std::shared_ptr<Light>,
                         glm::mat4> > & vec) const;

  // gets all child cameras, paired with their M matrix
  virtual void getCameras(const glm::mat4 & inM,
                          std::vector<std::pair<std::shared_ptr<Camera>,
                          glm::mat4> > & vec) const;

  // gets all child drawables, paired with their M matrix
  virtual void getDrawables(const glm::mat4 & inM,
                            std::vector<std::pair<std::shared_ptr<Drawable>,
                            glm::mat4> > & vec) const;

  // call the updateFn of all children with the current time (usually
  // from glfwGetTime())
  virtual void update(double time);

private:
  std::vector<std::shared_ptr<Node> > children;
};

// Applies an arbitrary transformation to 1 child
class Transform : public Group
{
public:
  Transform(std::shared_ptr<Node> what,
            const glm::mat4 & with,
            std::function<void(glm::mat4 &, double)> uFn = doNothing);

  bool isTransform() const {return true;}

  virtual void insert(std::shared_ptr<Node> what) {target = what;}
  virtual void remove(std::shared_ptr<Node> what)
  {
    if (target == what) target.reset();
  }

  virtual void clear() {target.reset();}

  virtual void getLights(const glm::mat4 & inM,
                         std::vector<std::pair<std::shared_ptr<Light>,
                         glm::mat4> > & vec) const;

  virtual void getCameras(const glm::mat4 & inM,
                          std::vector<std::pair<std::shared_ptr<Camera>,
                          glm::mat4> > & vec) const;

  virtual void getDrawables(const glm::mat4 & inM,
                            std::vector<std::pair<std::shared_ptr<Drawable>,
                            glm::mat4> > & vec) const;

  void setTransform(glm::mat4 toSet) {xform = toSet;}
  void setUpdateFn(std::function
                   <void(glm::mat4 &, double)> uFn = doNothing)
  {
    updateFn = uFn;
  }

  // calls the updateFn
  virtual void update(double time);

private:
  glm::mat4 toWorld() const {return xform;}
  glm::mat4 xform;

  std::function<void(glm::mat4 & xform, double time)> updateFn;
  std::shared_ptr<Node> target;
};

// Walks the scenegraph, gets all lights paired with their M matrix
std::vector<std::pair<std::shared_ptr<Light>,
                      glm::mat4> > getLights(const Group & root);

// Walks the scenegraph, gets all cameras paired with their M matrix
std::vector<std::pair<std::shared_ptr<Camera>,
                      glm::mat4> > getCameras(const Group & root);

// Walks the scenegraph, gets all drawables paired with their M matrix
std::vector<std::pair<std::shared_ptr<Drawable>,
                      glm::mat4> > getDrawables(const Group & root);

// The draw function takes a vector of (shared_ptr<Light>, M matrix),
// a *single* (shared_ptr<camera>, M Matrix) pair, and a vector of
// (shared_ptr<drawable>, M matrix)
//
// drawFn should implement all the boilerplate needed to draw the scene
typedef std::function<void(const std::vector<std::pair<std::shared_ptr<Light>,
                           glm::mat4> > &,
                           const std::pair<std::shared_ptr<Camera>,
                           glm::mat4> &,
                           const std::pair<std::shared_ptr<Drawable>,
                           glm::mat4> &)> DrawFn;

// draws the scene. Workflow:
// 1) call getCameras
// 2) pick the camera to use to draw. Multiple cameras are useful for multiple
//    viewpoints, or to do things like split screen
// 3) call draw([your drawFn], [scene graph root], [the camera to use])
// 4) repeat with other cameras as nessesary
void draw(DrawFn drawFn,
          const Group & root,
          const std::pair<std::shared_ptr<Camera>, glm::mat4> & camera);


glm::mat4 orbit(glm::vec3 from, glm::vec3 to, float sf);
glm::mat4 move(glm::vec3 from, glm::vec3 to, float sf);

#endif
