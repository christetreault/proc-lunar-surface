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
in float depositValToFrag;

uniform sampler2D stoneTex;
uniform sampler2D gravelTex;
uniform sampler2D depositTex;

uniform vec4 lightDir;
uniform vec3 lightColor;

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

  vec4 specular = vec4(0.0);
  if (intensity > 0.0)
    {
      vec3 posInv = normalize(-vec3(posToFrag));
      vec3 half = normalize(dir + posInv);
      specular = vec4(lightColor, 1.0)
        * mat.specular
        * pow(max(dot(normal, half), 0.0), mat.shininess);
    }

  return tex * (ambient + diffuse + specular);
}

void main()
{
  Material pearl = Material(vec4( vec3(0.55, 0.50725, 0.50725), 1.0),
                            vec4(1.0, 0.829, 0.829, 1.0),
                            vec4(0.296648, 0.296648, 0.296648, 1.0),
                            11.264 ); // gravel
  Material obsidian = Material(vec4(0.35375, 0.35, 0.36625, 1.0),
                               vec4(0.28275, 0.27, 0.32525, 1.0),
                               vec4(0.132741, 0.128634, 0.146435, 1.0),
                               38.4); // rock
  Material ruby = Material(vec4(0.4745, 0.31175, 0.31175, 1.0),
                           vec4(0.61424, 0.04136, 0.04136, 1.0),
                           vec4(0.927811, 0.826959, 0.826959, 1.0),
                           76.8); // deposit

  vec3 normal = normalize(normalToFrag);
  vec3 straightUp = normalize(vec3(0.0, 1.0, 0.0));
  vec3 straightDown = normalize(vec3(0.0, -1.0, 0.0));

  float upAngle = acos(dot(normal, straightUp));
  float downAngle = acos(dot(normal, straightDown));
  float thresh = 0.4;
  float threshUpper = thresh + 0.2;
  float angle = min(upAngle, downAngle);

  vec4 litDeposit = lightTexture(texture(depositTex, texCoordToFrag),
                                 ruby);
  vec4 litStone = lightTexture(texture(stoneTex, texCoordToFrag),
                               obsidian);
  vec4 litGravel = lightTexture(texture(gravelTex, texCoordToFrag),
                                pearl);

  vec4 selectStoneTex;
  if (depositValToFrag > 0.5)
    {
      selectStoneTex = litDeposit;
    }
  else if (depositValToFrag > 0.2)
    {
      float t = mapRange(depositValToFrag, 0.2, 0.5, 0.0, 1.0);
      selectStoneTex = mix(litStone,
                           litDeposit,
                           t);
    }
  else
    {
      selectStoneTex = litStone;
    }

  if (angle < thresh)
    {
      color = litGravel;
    }
  else if (angle < threshUpper)
    {
      float t = mapRange(angle, thresh, threshUpper, 0.0, 1.0);
      color = mix(litGravel,
                  selectStoneTex,
                  t);
    }
  else
    {
      color = selectStoneTex;
    }
}
