#ifndef SHADER_HPP
#define SHADER_HPP

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <functional>

class Shader
{
public:
  Shader(const char * vert, const char * frag);
  ~Shader();
  bool isCurrent() const;
  // bind this shader. The uniformFn parameter is a lambda
  // that sets up all the uniforms correctly
  void bind(std::function<void(GLuint)> uniformFn);
  void unbind();
  GLuint getId() {return id;}
private:
  Shader(const Shader&) = delete;
  Shader & operator=(const Shader&) = delete;

  static Shader * curr;
  GLuint id;
  GLuint LoadShaders(const char * vertex_file_path,
                     const char * fragment_file_path) const;
};
#endif
