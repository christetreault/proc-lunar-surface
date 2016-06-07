#version 330 core

struct Material
{
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float shininess;
};

in highp vec3 normalToFrag;
in highp vec3 posToFrag;
in highp vec3 viewToFrag;
in highp vec3 innerTexCoordToFrag;
in highp vec3 outerTexCoordToFrag;

uniform highp vec3 cameraPos;
uniform highp vec4 lightDir;
uniform highp vec3 lightColor;
uniform samplerCube tex;

uniform highp float time;

out lowp vec4 color;

highp float mapRange(highp float s, highp float sMin, highp float sMax,
               highp float tMin, highp float tMax)
{
  return tMin + (((s - sMin) * (tMax - tMin)) / (sMax - sMin));
}

highp vec4 lightTexture(highp vec4 tex, Material mat)
{
  // ambient
  highp vec4 ambient = vec4(lightColor, 1.0) * mat.ambient;

  // diffuse

  highp vec3 dir = vec3(normalize(-lightDir));
  highp vec3 normal = normalize(normalToFrag);

  highp float intensity = max(dot(normal, dir), 0.0);
  highp vec4 diffuse = intensity * vec4(lightColor, 1.0) * mat.diffuse;

  // specular

  highp vec4 specular = vec4(0.0);
  if (intensity > 0.0)
    {
      highp vec3 posInv = normalize(-vec3(posToFrag));
      highp vec3 half_ = normalize(dir + posInv);
      specular = vec4(lightColor, 1.0)
        * mat.specular
        * pow(max(dot(normal, half_), 0.0), mat.shininess);
    }

  return  tex * (ambient + diffuse + specular);
}

void main()
{
  //Material crystal = Material(highp vec4(0.8, 0.8, 0.8, 1.0),
  //                            highp vec4(1.0, 1.0, 1.0, 1.0),
  //                            highp vec4(1.0, 1.0, 1.0, 1.0),
  //                            76.8);
  Material ruby = Material(vec4(0.6745, 0.51175, 0.51175, 1.0),
                           vec4(0.61424, 0.04136, 0.04136, 1.0),
                           vec4(0.927811, 0.826959, 0.826959, 1.0),
                           76.8);
  highp vec3 texCoord = vec3(innerTexCoordToFrag.x + cos(time),
                       innerTexCoordToFrag.y + cos(time),
                       innerTexCoordToFrag.z);

  highp vec4 usingNormal = texture(tex, normalize(outerTexCoordToFrag));
  lowp vec4 surfaceColor = vec4((usingNormal.x + usingNormal.y + usingNormal.z) / 3.0,
                           (usingNormal.x + usingNormal.y + usingNormal.z) / 3.0,
                           (usingNormal.x + usingNormal.y + usingNormal.z) / 3.0,
                           1.0) * 2.0;

  highp float edge = max(0.0, dot(normalize(normalToFrag),
                            normalize(cameraPos)));

  lowp vec4 baseColor = lightTexture(mix(surfaceColor,
                                    texture(tex, texCoord),
                                    0.8),
                                ruby);
  lowp vec4 black = vec4(0.0,0.0,0.0,1.0);
  if (edge > 0.2)
    {
      color = lightTexture(mix(surfaceColor,
                                    texture(tex, texCoord),
                                    0.8),
                                ruby);
    }
  else
    {
      color = lightTexture(mix(surfaceColor,
                               texture(tex, texCoord),
                               mapRange(edge, 0.2, 0.0, 0.8, 0.4)),
                           ruby);
    }
}
