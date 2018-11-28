#include "shader.h"

void loadShader(const GLchar* filePath, GLchar** shader)
{
  FILE *f = fopen(filePath, "rb");
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);  //same as rewind(f);

  *shader = malloc(fsize + 1);
  fread(*shader, fsize, 1, f);
  fclose(f);

  (*shader)[fsize] = 0;
}

void compileShaders(const GLchar* vertexPath, const GLchar* fragmentPath, GLuint* shaderID)
{
  GLuint sVertex, sFragment;
  GLchar* vertexSource;
  GLchar* fragmentSource;
  loadShader(vertexPath, &vertexSource);
  loadShader(fragmentPath, &fragmentSource);

  // Vertex Shader
  sVertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(sVertex, 1, (const GLchar **)&vertexSource, NULL);
  glCompileShader(sVertex);
  checkCompileErrors(sVertex, "VERTEX");

  // Fragment Shader
  sFragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(sFragment, 1, (const GLchar **)&fragmentSource, NULL);
  glCompileShader(sFragment);
  checkCompileErrors(sFragment, "FRAGMENT");

  // Shader Program
  *shaderID = glCreateProgram();
  glAttachShader(*shaderID, sVertex);
  glAttachShader(*shaderID, sFragment);
  glLinkProgram(*shaderID);
  checkCompileErrors(*shaderID, "PROGRAM");

  // Delete the shaders as they're linked into our program now and no longer necessery
  glDeleteShader(sVertex);
  glDeleteShader(sFragment);
}

void checkCompileErrors(GLuint object, const char* type)
{
  GLint success;
  GLchar infoLog[1024];
  if (strcmp(type, "PROGRAM") != 0)
  {
    glGetShaderiv(object, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(object, 1024, NULL, infoLog);
      printf("Error shader of type %s\n %s\n", type, infoLog);
    }
  }
  else
  {
    glGetProgramiv(object, GL_LINK_STATUS, &success);
    if (!success)
    {
      glGetProgramInfoLog(object, 1024, NULL, infoLog);
      printf("Error linking shader of type %s\n %s\n", type, infoLog);
    }
  }
}
