#include "SkyBox.hpp"

// From Cube.cpp provided with PA2
// http://ivl.calit2.net/wiki/index.php/BasecodeCSE167S16
static GLfloat vertices[] = {
	// Front vertices
	-0.5, -0.5,  0.5,
	0.5, -0.5,  0.5,
	0.5,  0.5,  0.5,
	-0.5,  0.5,  0.5,
	// Back vertices
	-0.5, -0.5, -0.5,
	0.5, -0.5, -0.5,
	0.5,  0.5, -0.5,
	-0.5,  0.5, -0.5
};

static GLuint indices[] = {  // Note that we start from 0!
	// Front face
	2, 1, 0,
	0, 3, 2,
	// Top face
	6, 5, 1,
	1, 2, 6,
	// Back face
	5, 6, 7,
	7, 4, 5,
	// Bottom face
	3, 0, 4,
	4, 7, 3,
	// Left face
	1, 5, 4,
	4, 0, 1,
	// Right face
	6, 2, 3,
	3, 7, 6
};


SkyBox::SkyBox(std::shared_ptr<Shader> inShader,
               GLenum inTexUnit,
               const char * right,
               const char * left,
               const char * top,
               const char * bottom,
               const char * back,
               const char * front)
{
  shader = inShader;
  texUnit = inTexUnit;
  faces[0] = Texture::getTexture(right);
  faces[1] = Texture::getTexture(left);
  faces[2] = Texture::getTexture(top);
  faces[3] = Texture::getTexture(bottom);
  faces[4] = Texture::getTexture(back);
  faces[5] = Texture::getTexture(front);

  glGenTextures(1, &id);
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glActiveTexture(texUnit);
  glBindTexture(GL_TEXTURE_CUBE_MAP, id);

  for(GLuint i = 0; i < 6; ++i)
    {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                   0,
                   GL_RGB,
                   faces[i]->width,
                   faces[i]->height,
                   0,
                   GL_RGB,
                   GL_UNSIGNED_BYTE,
                   faces[i]->bytes);
    }

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // The following copied from the Cube.cpp provided with PA2
  // http://ivl.calit2.net/wiki/index.php/BasecodeCSE167S16

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


  glVertexAttribPointer(0,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        3 * sizeof(GLfloat),
                        (GLvoid*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  unbind();
  valid = true;
}

bool SkyBox::isValid() const
{
  return valid;
}

void SkyBox::bind()
{
  assert (valid);
  glActiveTexture(texUnit);
  glBindTexture(GL_TEXTURE_CUBE_MAP, id);
  glBindVertexArray(VAO);
}

void SkyBox::unbind()
{
  glActiveTexture(texUnit);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  glBindVertexArray(0);
}

SkyBox::~SkyBox()
{
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}
