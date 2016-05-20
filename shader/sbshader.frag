#version 330 core

uniform samplerCube skybox;


in vec3 texCoordsToFrag;
out vec4 color;

void main()
{
  color = texture(skybox, texCoordsToFrag);
}
