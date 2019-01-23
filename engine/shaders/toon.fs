#version 330 core
#define NR_LIGHTS 4

out vec4 FragColor;

struct Material {
  vec3 diffuse;
  vec3 specular;
}; 

in vec3 Normal;  
in vec2 Uvs;
in vec3 FragPos;  
in vec4 FragPosLightSpace;

// lights
uniform vec3 lightsPos[NR_LIGHTS]; 
uniform vec3 lightsColors[NR_LIGHTS];
uniform int lightsNr;

// camera
uniform vec3 cameraPos; 

// material
uniform Material material;

// shadow map
uniform sampler2D shadowMap;
uniform float shadowBias;

// texture
uniform int hasTexture;
uniform sampler2D texture1;

// time
uniform float time;

// render params
uniform int glowing;
uniform vec3 glow_color;
uniform int receive_shadows;

float shadowCalculation(vec4 fragPosLightSpace, vec3 lightDir) {
  // perform perspective divide
  vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
  // transform to [0,1] range
  projCoords = projCoords * 0.5 + 0.5;
  // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
  float closestDepth = texture(shadowMap, projCoords.xy).r; 
  // get depth of current fragment from light's perspective
  float currentDepth = projCoords.z;
  // check whether current frag pos is in shadow
  //float bias = max(shadowBias * (1.0 - dot(Normal, lightDir)), 0.02);
  float bias = 0.005;
  float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

  if (projCoords.z > 1.0)
    shadow = 0.0;

  return shadow;
}

void main() {
  vec3 result = vec3(0.0);
  for (int i = 0; i < min(NR_LIGHTS, lightsNr); i++) {
    // ambient
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightsColors[i];

    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightsPos[i] - FragPos);
    float diff = max(dot(norm, lightDir), 0.8);
    vec3 diffuse = diff * lightsColors[i] * material.diffuse;

    // specular
    float specularStrength = 0.2;
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    spec = spec > 0.01 ? 0.6 : 0;
    vec3 specular = specularStrength * spec * lightsColors[i];  
   
    // shadows
    float shadow = 0.0;
    if (receive_shadows == 1) {
      shadow = shadowCalculation(FragPosLightSpace, lightDir);
    }

    vec3 objectColor = hasTexture > 0 ? texture(texture1, Uvs).rgb : vec3(1.0);
    result += (ambient + (1.0 - shadow) * (diffuse + specular)) * objectColor;
    //result = texture(shadowMap, FragPos.xy).rgb;
  }

  // glowing effect
  if (glowing == 1) {
    result += vec3((1.0f + sin(time * 5.0f)) / 8.0f);
    result *= glow_color;
  }

  FragColor = vec4(result, 1.0f);
} 
