#version 300 es

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

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
