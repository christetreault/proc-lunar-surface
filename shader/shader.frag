#version 330 core

struct Material
{
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

uniform Material material;

in vec3 normalToFrag;
in vec3 posToFrag;

out vec4 color;


void main()
{
  color = vec4(normalToFrag, 1.0f);
}
