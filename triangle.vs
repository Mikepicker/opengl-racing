#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;

out vec3 ourColor;

void main()
{
  gl_Position = vec4(pos, 1.0);
  ourColor = color;
}
