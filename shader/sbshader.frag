#version 300 es

uniform samplerCube skybox;


in mediump vec3 texCoordsToFrag;
out lowp vec4 color;

void main()
{
  color = texture(skybox, texCoordsToFrag);
}
