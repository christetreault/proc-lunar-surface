#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <iostream>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <typeinfo>
#include "SceneGraph.hpp"
#include "Scene.hpp"
#include "Util.hpp"

class Window
{
public:
  static int width;
  static int height;
  static void initialize_objects();
  static void clean_up();
  static GLFWwindow* create_window(int width,
                                   int height);
  static void resize_callback(GLFWwindow* window,
                              int width,
                              int height);
  static void idle_callback();
  static void display_callback(GLFWwindow*);
  static void key_callback(GLFWwindow* window,
                           int key,
                           int scancode,
                           int action,
                           int mods);

  static void mouseCallback(GLFWwindow * window,
                            int button,
                            int action,
                            int mods);
  static void mousePosCallback(GLFWwindow * window,
                               double x,
                               double y);
  static void scrollCallback(GLFWwindow* window,
                             double xoffset,
                             double yoffset);
};

#endif
