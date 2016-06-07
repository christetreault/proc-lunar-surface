#version 330 core

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
in highp vec4 lightCoordToFrag;
in highp float depositValToFrag;

uniform sampler2D stoneTex;
uniform sampler2D gravelTex;
uniform sampler2D depositTex;
uniform sampler2D shadowMapTex;

uniform highp vec4 lightDir;
uniform lowp vec3 lightColor;

out lowp vec4 color;

highp float mapRange(highp float s, highp float sMin, highp float sMax,
               highp float tMin, highp float tMax)
{
  return tMin + (((s - sMin) * (tMax - tMin)) / (sMax - sMin));
}

highp vec4 lightTexture(highp vec4 tex, Material mat, bool in_shadow)
{
  // ambient
  highp vec4 ambient = vec4(lightColor, 1.0) * mat.ambient;

  // diffuse

  highp vec3 dir = vec3(normalize(-lightDir));
  highp vec3 normal = normalize(normalToFrag);

  highp float intensity = max(dot(normal, dir), 0.0);
  highp vec4 diffuse = intensity * vec4(lightColor, 1.0) * mat.diffuse;

  //  return highp vec4(highp vec3(1,0,1)*intensity, 1);

  highp vec4 specular = vec4(0.0);
  if (intensity > 0.0)
    {
      highp vec3 posInv = normalize(-vec3(posToFrag));
      highp vec3 half_ = normalize(dir + posInv);
      specular = vec4(lightColor, 1.0)
        * mat.specular
        * pow(max(dot(normal, half_), 0.0), mat.shininess);
    }

  //if (diffuse == 0.0) return highp vec4(normal, 1.0);
  if(in_shadow) return tex*ambient;
  return tex * (ambient + diffuse + specular);
  //return specular;
}

void main()
{
  Material pearl = Material( vec4( vec3(0.55, 0.50725, 0.50725), 1.0),
                             vec4(1.0, 0.829, 0.829, 1.0),
                             vec4(0.296648, 0.296648, 0.296648, 1.0),
                            11.264 ); // gravel
  Material obsidian = Material( vec4(0.35375, 0.35, 0.36625, 1.0),
                                vec4(0.28275, 0.27, 0.32525, 1.0),
                                vec4(0.132741, 0.128634, 0.146435, 1.0),
                               38.4); // rock
  Material ruby = Material( vec4(0.4745, 0.31175, 0.31175, 1.0),
                            vec4(0.61424, 0.04136, 0.04136, 1.0),
                            vec4(0.927811, 0.826959, 0.826959, 1.0),
                           76.8); // deposit

  highp vec3 lsc = lightCoordToFrag.xyz/lightCoordToFrag.w * 0.5 + 0.5;


  highp float z = texture(shadowMapTex, lsc.xy).z;
  bool in_shadow = (z < lsc.z - 0.002);
  if(z == 0.0) in_shadow = false;

  highp vec3 normal = normalize(normalToFrag);
  highp vec3 straightUp = normalize(vec3(0.0, 1.0, 0.0));
  highp vec3 straightDown = normalize(vec3(0.0, -1.0, 0.0));

  highp float upAngle = acos(dot(normal, straightUp));
  highp float downAngle = acos(dot(normal, straightDown));
  highp float thresh = 0.4;
  highp float threshUpper = thresh + 0.2;
  highp float angle = min(upAngle, downAngle);

  highp vec4 litDeposit = lightTexture(texture(depositTex, texCoordToFrag),
                                 ruby, in_shadow);
  highp vec4 litStone = lightTexture(texture(stoneTex, texCoordToFrag),
                               obsidian, in_shadow);
  highp vec4 litGravel = lightTexture(texture(gravelTex, texCoordToFrag),
                                pearl, in_shadow);

  highp vec4 selectStoneTex;
  if (depositValToFrag > 0.5)
    {
      selectStoneTex = litDeposit;
    }
  else if (depositValToFrag > 0.2)
    {
      highp float t = mapRange(depositValToFrag, 0.2, 0.5, 0.0, 1.0);
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
      highp float t = mapRange(angle, thresh, threshUpper, 0.0, 1.0);
      color = mix(litGravel,
                  selectStoneTex,
                  t);
    }
  else
    {
      color = selectStoneTex;
    }
    //color = texture(shadowMapTex, texCoordToFrag+highp vec2(0.5,0.5));
}
