#include "shader.h"

void shader_check_compile_errors(GLuint object, const char* type)
{
  GLint success;
  GLchar info_log[1024];
  if (strcmp(type, "PROGRAM") != 0)
  {
    glGetShaderiv(object, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(object, 1024, NULL, info_log);
      printf("Error shader of type %s\n %s\n", type, info_log);
    }
  }
  else
  {
    glGetProgramiv(object, GL_LINK_STATUS, &success);
    if (!success)
    {
      glGetProgramInfoLog(object, 1024, NULL, info_log);
      printf("Error linking shader of type %s\n %s\n", type, info_log);
    }
  }
}

void shader_load(const GLchar* file_path, GLchar** shader)
{
  FILE *f = fopen(file_path, "rb");
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);  //same as rewind(f);

  *shader = malloc(fsize + 1);
  fread(*shader, fsize, 1, f);
  fclose(f);

  (*shader)[fsize] = 0;
}

void shader_compile(const GLchar* vertex_path, const GLchar* fragment_path, GLuint* shader_id)
{
  GLuint s_vertex, s_fragment;
  GLchar* vertex_source;
  GLchar* fragment_source;
  shader_load(vertex_path, &vertex_source);
  shader_load(fragment_path, &fragment_source);

  // Vertex Shader
  s_vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(s_vertex, 1, (const GLchar **)&vertex_source, NULL);
  glCompileShader(s_vertex);
  shader_check_compile_errors(s_vertex, "VERTEX");

  // Fragment Shader
  s_fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(s_fragment, 1, (const GLchar **)&fragment_source, NULL);
  glCompileShader(s_fragment);
  shader_check_compile_errors(s_fragment, "FRAGMENT");

  // Shader Program
  *shader_id = glCreateProgram();
  glAttachShader(*shader_id, s_vertex);
  glAttachShader(*shader_id, s_fragment);
  glLinkProgram(*shader_id);
  shader_check_compile_errors(*shader_id, "PROGRAM");

  // Delete the shaders as they're linked into our program now and no longer necessery
  glDeleteShader(s_vertex);
  glDeleteShader(s_fragment);
}
