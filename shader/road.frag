#version 330 core

struct Material
{
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

uniform Material material;

in vec3 posToFrag;

out vec4 color;

void main()
{
  color = vec4(posToFrag, 1.0f);
  color = vec4(1,0,1,1.0f);
}
