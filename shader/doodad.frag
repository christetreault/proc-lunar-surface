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
in vec3 innerTexCoordToFrag;
in vec3 outerTexCoordToFrag;

uniform vec4 lightDir;
uniform vec3 lightColor;
uniform samplerCube tex;

uniform float time;

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
  //color = vec4(normalize(normalToFrag), 1.0f);
  //return;
  Material crystal = Material(vec4(0.8, 0.8, 0.8, 1.0),
                              vec4(1.0, 1.0, 1.0, 1.0),
                              vec4(1.0, 1.0, 1.0, 1.0),
                              76.8);
  vec3 texCoord = vec3(innerTexCoordToFrag.x + cos(time),
                       innerTexCoordToFrag.y + cos(time / 2.0),
                       innerTexCoordToFrag.z);

  vec4 usingNormal = texture(tex, normalize(outerTexCoordToFrag));
  vec4 surfaceColor = vec4((usingNormal.x + usingNormal.y + usingNormal.z) / 3,
                           (usingNormal.x + usingNormal.y + usingNormal.z) / 3,
                           (usingNormal.x + usingNormal.y + usingNormal.z) / 3,
                           1.0) * 2.0;

  color = lightTexture(mix(surfaceColor,
                           texture(tex, texCoord),
                           0.9),
                       crystal);
}
