#version 330 core

struct Material
{
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float shininess;
};

in vec3 normalToFrag;
in vec3 posToFrag;
in vec2 texCoordToFrag;
in float screenspaceZ;
in float depositValToFrag;

uniform sampler2D stoneTex;
uniform sampler2D gravelTex;
uniform sampler2D depositTex;

uniform vec4 lightDir;
uniform vec3 lightColor;

out vec4 color;


void main()
{
   float z = -screenspaceZ;
   color = vec4(z,z,z, 1);
}
