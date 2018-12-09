#version 330 core
out vec4 fragColor;

in vec2 uv;

uniform sampler2D texture1;

void main ()
{
  fragColor = texture(texture1, uv);
  //fragColor = vec4(uv.x, 0.0, 0.0, 1.0);
}
