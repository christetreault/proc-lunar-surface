#ifndef SCENE_H
#define SCENE_H

#include "SceneGraph.hpp"
#include "Landscape.hpp"
#include "Light.hpp"
#include "SkyBox.hpp"
#include <memory>
#include <glm/gtx/vector_angle.hpp>
#include <glm/glm.hpp>

// top level code to build a scene graph. Not strictly needed,
// but reduces clutter in Window.cpp
std::shared_ptr<Group> getScene(std::shared_ptr<Camera>);

// returns a DrawFn used by SceneGraph::draw (see SceneGraph.hpp for
// what the signature of DrawFn is)
DrawFn getDrawFn (const glm::mat4 & P);

#endif
