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

// lights
uniform vec3 lightsPos[NR_LIGHTS]; 
uniform vec3 lightsColors[NR_LIGHTS];
uniform int lightsNr;

// camera
uniform vec3 cameraPos; 

// material
uniform Material material;

// texture
uniform int hasTexture;
uniform sampler2D texture1;

// time
uniform float time;

// render params
uniform int glowing;
uniform vec3 glow_color;

void main()
{
  vec3 result = vec3(0.0);
  for (int i = 0; i < min(NR_LIGHTS, lightsNr); i++) {
    // ambient
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightsColors[i];

    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightsPos[i] - FragPos);
    float diff = max(dot(norm, lightDir), 0.8);
    // float diff = smoothstep(0.0, 0.0, max(dot(norm, lightDir), 0.0));
    // float diff = max(dot(norm, lightDir), 0.0) > 0 ? 1 : 0.8;
    vec3 diffuse = diff * lightsColors[i] * material.diffuse;

    // specular
    float specularStrength = 0.2;
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    // spec = smoothstep(0.005, 0.01, spec);
    spec = spec > 0.01 ? 0.6 : 0;
    vec3 specular = specularStrength * spec * lightsColors[i];  
   
    // rimdot
    float rimdot = 1 - dot(viewDir, norm);
    float rimAmount = 0.71;
    float rimIntensity = smoothstep(rimAmount - 0.01, rimAmount + 0.01, rimdot);
    vec3 rim = rimIntensity * vec3(0);

    vec3 objectColor = hasTexture > 0 ? texture(texture1, Uvs).rgb : vec3(1.0);
    result += (ambient + diffuse + specular) * objectColor;
    // result = vec3(diff);
  }

  // glowing effect
  if (glowing == 1) {
    result += vec3((1.0f + sin(time * 5.0f)) / 8.0f);
    result *= glow_color;
  }

  FragColor = vec4(result, 1.0f);
} 
