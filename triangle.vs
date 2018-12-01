#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 uvs;
layout (location = 2) in vec3 norm;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main()
{
  gl_Position = P * V * M * vec4(pos, 1.0f);
}
