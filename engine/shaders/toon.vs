#version 330 core
#define NR_LIGHTS 4

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUvs;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;

out vec3 FragPos;
out vec2 Uvs;
out vec3 Normal;
out vec4 FragPosLightSpace;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 lightSpaceMatrix;

out VS_OUT {
  vec3 TangentLightPos;
  vec3 TangentViewPos;
  vec3 TangentFragPos;
  vec3 debug;
} vs_out;

uniform vec3 cameraPos; 
uniform vec3 lightsPos[NR_LIGHTS]; 
uniform int hasNormalMap;

void main() {
  FragPos = vec3(M * vec4(aPos, 1.0));
  Uvs = aUvs.st;
  Normal = mat3(transpose(inverse(M))) * aNormal;  
  FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

  // normal map
  if (hasNormalMap > 0) {
    mat3 normalMatrix = transpose(inverse(mat3(M)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N));    
    vs_out.TangentLightPos = TBN * lightsPos[0];
    vs_out.TangentViewPos  = TBN * cameraPos;
    vs_out.TangentFragPos  = TBN * FragPos;
    vs_out.debug = aTangent;
  }

  gl_Position = P * V * vec4(FragPos, 1.0);
}
