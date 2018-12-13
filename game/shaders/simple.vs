#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uvs;
layout (location = 2) in vec3 norm;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec2 uv;

void main()
{
  gl_Position = P * V * M * vec4(pos, 1.0f);
  uv = vec2(uvs.x, uvs.y);
}
