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
uniform int shadowPCFEnabled;

// skybox
uniform samplerCube skybox;

// texture
uniform int hasTexture;
uniform sampler2D texture1;
uniform int texture_subdivision;

// time
uniform float time;

// render params
uniform vec3 color_mask;
uniform int glowing;
uniform vec3 glow_color;
uniform int receive_shadows;

// normal map
uniform int hasNormalMap;
uniform sampler2D normalMap;
in VS_OUT {
  vec3 TangentLightPos;
  vec3 TangentViewPos;
  vec3 TangentFragPos;
  vec3 debug;
} fs_in;

// specular map
uniform int hasSpecularMap;
uniform sampler2D specularMap;

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
  float bias = max(0.02 * (1.0 - dot(Normal, lightDir)), 0.01);

  float shadow = 0.0;
  
  if (shadowPCFEnabled == 1) {
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; x++) {
      for (int y = -1; y <= 1; y++) {
        float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
      }
    }
    shadow /= 9.0;
  } else {
    shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
  }

  if (projCoords.z > 1.0)
    shadow = 0.0;

  return shadow;
}

vec3 computeNormal()
{
  // obtain normal from normal map in range [0,1]
  vec3 normal = texture(normalMap, Uvs * texture_subdivision).rgb;

  // transform normal vector to range [-1,1]
  normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

  return normal;
}

void main() {
  vec3 result = vec3(0.0);
  for (int i = 0; i < min(NR_LIGHTS, lightsNr); i++) {
    // ambient
    float ambientStrength = 0.8;
    vec3 ambient = ambientStrength * lightsColors[i];

    // diffuse 
    // vec3 norm = hasNormalMap > 0 ? computeNormal() : normalize(Normal);
    vec3 norm = hasNormalMap > 0 ? computeNormal() : normalize(Normal);
    vec3 lightDir = hasNormalMap > 0 ? normalize(fs_in.TangentLightPos - fs_in.TangentFragPos) : normalize(lightsPos[i] - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightsColors[i];

    // specular
    float specularStrength = 0.2;
    vec3 viewDir = hasNormalMap > 0 ? normalize(fs_in.TangentViewPos - fs_in.TangentFragPos) : normalize(cameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 64);
    spec = spec > 0.01 ? 0.6 : 0;

    // skybox
    vec3 r = reflect(-viewDir, normalize(norm));
    vec3 specular = specularStrength * spec * lightsColors[i] * material.specular + texture(skybox, r).rgb * material.specular;  

    if (hasSpecularMap > 0) {
      specular *= texture(specularMap, Uvs).rgb;
    }
   
    // shadows
    float shadow = 0.0;
    if (receive_shadows == 1) {
      shadow = shadowCalculation(FragPosLightSpace, lightDir);
    }

    vec3 objectColor = hasTexture > 0 ? texture(texture1, Uvs * texture_subdivision).rgb : material.diffuse;
    result += (ambient + (1.0 - shadow) * (diffuse + specular)) * objectColor;
  }

  // color mask
  result += color_mask;

  // glowing effect
  if (glowing == 1) {
    result += vec3((1.0f + sin(time * 5.0f)) / 8.0f);
    result *= glow_color;
  }

  FragColor = vec4(result, 1.0f);
} 
