#version 330 core

layout (location = 0) in vec3 positionToVert;

uniform mat4 PV;
uniform mat4 M;

out vec3 texCoordsToFrag;

void main()
{
  gl_Position = PV * M * vec4(positionToVert,
                              1.0);
  texCoordsToFrag = positionToVert;
}
