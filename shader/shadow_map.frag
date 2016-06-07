#version 300 es

struct Material
{
  highp vec4 ambient;
  highp vec4 diffuse;
  highp vec4 specular;
  highp float shininess;
};

in highp vec3 normalToFrag;
in highp vec3 posToFrag;
in highp vec2 texCoordToFrag;
in highp float screenspaceZ;
in highp float depositValToFrag;

uniform highp vec4 lightDir;

out lowp vec4 color;

void main()
{
   highp float z = abs(screenspaceZ);
   color = vec4(z,z,z, 1);
}
