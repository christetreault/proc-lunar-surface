#include "SceneGraph.hpp"

// nonmember functions

void draw(DrawFn drawFn,
          const Group & root,
          const std::pair<std::shared_ptr<Camera>, glm::mat4> & camera)
{
  glClear(GL_DEPTH_BUFFER_BIT);

  auto lights = getLights(root);
  auto drawables = getDrawables(root);

  for (const auto & action : drawables)
    {
      drawFn(lights, camera, action);
    }
}

std::vector<std::pair<std::shared_ptr<Light>,
                      glm::mat4> > getLights(const Group & root)
{
  std::vector<std::pair<std::shared_ptr<Light>, glm::mat4> > vec;
  root.getLights(glm::mat4(), vec);
  return vec;
}

std::vector<std::pair<std::shared_ptr<Camera>,
                      glm::mat4> > getCameras(const Group & root)
{
  std::vector<std::pair<std::shared_ptr<Camera>, glm::mat4> > vec;
  root.getCameras(glm::mat4(), vec);
  return vec;
}

std::vector<std::pair<std::shared_ptr<Drawable>,
                      glm::mat4> > getDrawables(const Group & root)
{
  std::vector<std::pair<std::shared_ptr<Drawable>, glm::mat4> > vec;
  root.getDrawables(glm::mat4(), vec);
  return vec;
}

// Group

void Group::update(double time)
{
  for (const auto & curr : children)
    {
      if (curr->isGroup())
        {
          auto group = std::dynamic_pointer_cast<Group>(curr);
          group->update(time);
        }
    }
}

void Transform::update(double time)
{
  updateFn(xform, time);
  if (target->isGroup())
    {
      auto group = std::dynamic_pointer_cast<Group>(target);
      group->update(time);
    }
}

Transform::Transform(std::shared_ptr<Node> what,
                     const glm::mat4 & with,
                     std::function<void(glm::mat4 &, double)> uFn)
  : xform(with), updateFn(uFn), target(what)
{}

void Group::remove(std::shared_ptr<Node> what)
{
  auto iter = children.begin();

  while (iter != children.end())
    {
      if (*iter == what)
        {
          children.erase(iter);
        }
    }
}

void Group::getLights(const glm::mat4 & inM,
                          std::vector<std::pair<std::shared_ptr<Light>,
                                                glm::mat4> > & vec) const
{
  auto outM = inM;

  for (const auto & curr : children)
    {
      if (curr->isLight())
        {
          auto light = std::dynamic_pointer_cast<Light>(curr);
          vec.push_back(std::make_pair(light, outM));
        }
      else if (curr->isGroup())
        {
          auto group = std::dynamic_pointer_cast<Group>(curr);
          group->getLights(outM, vec);
        }
    }
}

void Transform::getLights(const glm::mat4 & inM,
                          std::vector<std::pair<std::shared_ptr<Light>,
                                                glm::mat4> > & vec) const
{
  auto outM = inM * toWorld();

  if (target->isLight())
    {
      auto light = std::dynamic_pointer_cast<Light>(target);
      vec.push_back(std::make_pair(light, outM));
    }
  else if (target->isGroup())
    {
      auto group = std::dynamic_pointer_cast<Group>(target);
      group->getLights(outM, vec);
    }
}


void Group::getCameras(const glm::mat4 & inM,
                       std::vector<std::pair<std::shared_ptr<Camera>,
                                             glm::mat4> > & vec) const
{
  auto outM = inM;

  for (const auto & curr : children)
    {
      if (curr->isCamera())
        {
          auto camera = std::dynamic_pointer_cast<Camera>(curr);
          vec.push_back(std::make_pair(camera, outM));
        }
      else if (curr->isGroup())
        {
          auto group = std::dynamic_pointer_cast<Group>(curr);
          group->getCameras(outM, vec);
        }
    }
}

void Transform::getCameras(const glm::mat4 & inM,
                       std::vector<std::pair<std::shared_ptr<Camera>,
                                             glm::mat4> > & vec) const
{
  auto outM = inM * toWorld();


  if (target->isCamera())
    {
      auto camera = std::dynamic_pointer_cast<Camera>(target);
      vec.push_back(std::make_pair(camera, outM));
    }
  else if (target->isGroup())
    {
      auto group = std::dynamic_pointer_cast<Group>(target);
      group->getCameras(outM, vec);
    }
}

void Group::getDrawables(const glm::mat4 & inM,
                         std::vector<std::pair<std::shared_ptr<Drawable>,
                                               glm::mat4> > & vec) const
{
  auto outM = inM;

  for (const auto & curr : children)
    {
      if (curr->isDrawable())
        {
          auto drawable = std::dynamic_pointer_cast<Drawable>(curr);
          vec.push_back(std::make_pair(drawable, outM));
        }
      else if (curr->isGroup())
        {
          auto group = std::dynamic_pointer_cast<Group>(curr);
          group->getDrawables(outM, vec);
        }
    }
}

void Transform::getDrawables(const glm::mat4 & inM,
                             std::vector<std::pair<std::shared_ptr<Drawable>,
                                         glm::mat4> > & vec) const
{
  auto outM = inM * toWorld();

  if (target->isDrawable())
    {
      auto drawable = std::dynamic_pointer_cast<Drawable>(target);
      vec.push_back(std::make_pair(drawable, outM));
    }
  else if (target->isGroup())
    {
      auto group = std::dynamic_pointer_cast<Group>(target);
      group->getDrawables(outM, vec);
    }
}

OBJDrawable::OBJDrawable(std::shared_ptr<OBJObject> inModel,
                         std::shared_ptr<Shader> inShader,
                         std::shared_ptr<Material> inMaterial)
  : model(inModel), shader(inShader), material(inMaterial)
{}

glm::mat4 OrbitalCamera::getV(const glm::mat4 & M) const
{
  auto newUp = glm::vec4(up, 0.0f);
  auto newPos = M * (orbit * move) * glm::vec4(pos, 1.0f);
  //newUp = M * newUp;

  return glm::lookAt(glm::vec3(newPos),
                     glm::vec3(0.0f, 0.0f, 0.0f),
                     glm::vec3(newUp));
}

glm::mat4 orbit(glm::vec3 from, glm::vec3 to, float sf)
{
  // based on algorithm provided in:
  // http://web.cse.ohio-state.edu/~crawfis/Graphics/VirtualTrackball.html

  auto velocity = glm::length(to - from);
  auto theta = velocity * sf;
  auto axis = glm::cross(from, to);

  glm::mat4 orbitMatrix;

  if (axis.x == 0.0f && axis.y == 0.0f && axis.z == 0.0f)
    {
      return glm::mat4();
    }

  orbitMatrix = glm::rotate(glm::mat4(),
                            theta,
                            axis);

  return orbitMatrix;
}

glm::mat4 move(glm::vec3 from, glm::vec3 to, float sf)
{
  auto tv = (to - from) * sf;
  glm::mat4 moveMatrix;

  if (tv.x == 0.0f && tv.y == 0.0f && tv.z == 0.0f)
    {
      moveMatrix = glm::mat4();
    }

  moveMatrix = glm::translate(glm::mat4(), tv);

  return moveMatrix;
}
