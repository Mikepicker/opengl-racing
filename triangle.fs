#version 330 core
out vec4 fragColor;
void main ()
{
  /* if (ourColor.r > ourColor.g && ourColor.r > ourColor.b) {
    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
  } else if (ourColor.g > ourColor.r && ourColor.g > ourColor.b) {
    fragColor = vec4(0.0, 1.0, 0.0, 1.0);
  } else {
    fragColor = vec4(0.0, 0.0, 1.0, 1.0);
  } */
  // fragColor = vec4(ourColor, 1.0);
  fragColor = vec4(1.0, 1.0, 0.0, 1.0);
}
