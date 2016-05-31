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
in vec3 texCoordToFrag;

uniform vec4 lightDir;
uniform vec3 lightColor;
uniform samplerCube tex;

out vec4 color;

float mapRange(float s, float sMin, float sMax,
               float tMin, float tMax)
{
  return tMin + (((s - sMin) * (tMax - tMin)) / (sMax - sMin));
}

vec4 lightTexture(vec4 tex, Material mat)
{
  // ambient
  vec4 ambient = vec4(lightColor, 1.0) * mat.ambient;

  // diffuse

  vec3 dir = vec3(normalize(-lightDir));
  vec3 normal = normalize(normalToFrag);

  float intensity = max(dot(normal, dir), 0.0);
  vec4 diffuse = intensity * vec4(lightColor, 1.0) * mat.diffuse;

  // specular

  vec4 specular = vec4(0.0);
  if (intensity > 0.0)
    {
      vec3 posInv = normalize(-vec3(posToFrag));
      vec3 half = normalize(dir + posInv);
      specular = vec4(lightColor, 1.0)
        * mat.specular
        * pow(max(dot(normal, half), 0.0), mat.shininess);
    }

  return  tex * (ambient + diffuse + specular);
}

void main()
{
  color = texture(tex, normalToFrag);
}
