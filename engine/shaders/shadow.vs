#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 M;

void main() {
  vec3 FragPos = vec3(M * vec4(aPos, 1.0));
  gl_Position = lightSpaceMatrix * vec4(FragPos, 1.0);
}  
