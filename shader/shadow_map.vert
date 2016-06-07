#version 300 es

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 positionToVert;
layout (location = 1) in vec3 normalToVert;
layout (location = 2) in vec2 texCoordToVert;
layout (location = 3) in float isDepositToVert;

uniform mat4 PV;
uniform mat4 M;

uniform vec4 lightDir;
uniform vec3 lightColor;

out vec3 normalToFrag;
out vec3 posToFrag;
out vec2 texCoordToFrag;
out float screenspaceZ;
out float depositValToFrag;

void main()
{
//    screenspacePos = PV * M * vec4(positionToVert.x,
//                                    positionToVert.y,
//                                    positionToVert.z,
//                                    1.0);
//    gl_Position = screenspacePos;
    gl_Position = PV*M*vec4(positionToVert.x,
                               positionToVert.y,
                                                  positionToVert.z,
                                                  1.0);
    screenspaceZ = -gl_Position.z;
}
