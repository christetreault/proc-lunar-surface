#version 330 core

in vec3 normalToFrag;
in vec3 posToFrag;
in vec2 texCoordToFrag;
in float heightToFrag;

out vec4 color;

void main()
{
  color = vec4(normalize(normalToFrag), 1.0f);
}
