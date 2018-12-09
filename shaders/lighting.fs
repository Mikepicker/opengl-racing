#version 330 core
#define NR_LIGHTS 4

out vec4 FragColor;

in vec3 Normal;  
in vec2 Uvs;
in vec3 FragPos;  

uniform vec3 lightsPos[NR_LIGHTS]; 
uniform vec3 lightsColors[NR_LIGHTS];
uniform int lightsNr;
uniform vec3 cameraPos; 

uniform sampler2D texture1;

void main()
{
  vec3 result = vec3(0.0);
  for (int i = 0; i < min(NR_LIGHTS, lightsNr); i++) {
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightsColors[i];

    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightsPos[i] - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightsColors[i];

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightsColors[i];  

    vec3 objectColor = texture(texture1, Uvs).rgb;
    result += (ambient + diffuse + specular) * objectColor;
  }

  FragColor = vec4(result, 1.0);
} 
