#version 330 core

in vec3 normalToFrag;
in vec3 posToFrag;
in vec2 texCoordToFrag;
in float heightToFrag;

uniform sampler2D stoneTex;
uniform sampler2D gravelTex;

out vec4 color;

float mapRange(float s, float sMin, float sMax,
               float tMin, float tMax)
{
  return tMin + (((s - sMin) * (tMax - tMin)) / (sMax - sMin));
}

void main()
{
  vec3 normal = normalize(normalToFrag);
  vec3 straightUp = normalize(vec3(0.0, 1.0, 0.0));
  vec3 straightDown = normalize(vec3(0.0, -1.0, 0.0));

  float upAngle = acos(dot(normal, straightUp));
  float downAngle = acos(dot(normal, straightDown));
  float thresh = 0.5;
  float threshUpper = thresh + 0.2;
  float angle = min(upAngle, downAngle);

  if (angle < thresh)
    {
      color = texture(gravelTex, texCoordToFrag);
    }
  else if (angle < threshUpper)
    {
      float t = mapRange(angle, thresh, threshUpper, 0.0, 1.0);
      color = mix(texture(gravelTex, texCoordToFrag),
                  texture(stoneTex, texCoordToFrag),
                  t);
    }
  else
    {
      color = texture(stoneTex, texCoordToFrag);
    }
}
