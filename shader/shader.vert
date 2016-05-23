#version 330 core

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 positionToVert;
layout (location = 1) in vec3 normalToVert;
layout (location = 2) in vec2 texCoordToVert;

uniform mat4 PV;
uniform mat4 M;

out vec3 normalToFrag;
out vec3 posToFrag;

void main()
{
    gl_Position = PV * M * vec4(positionToVert.x,
                                positionToVert.y,
                                positionToVert.z,
                                1.0);
    normalToFrag = mat3(transpose(inverse(M))) * normalToVert;
    posToFrag = (vec3(M * vec4(positionToVert, 1.0f)));
}
