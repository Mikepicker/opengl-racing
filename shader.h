#ifndef shader_h
#define shader_h

#include "engine.h"

void loadShader(const GLchar* filePath, GLchar** shader);
void compileShaders(const GLchar* vertexPath, const GLchar* fragmentPath, GLuint* shaderID);
void checkCompileErrors(GLuint object, const char* type);

#endif
