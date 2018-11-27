#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
//#include <cglm/cglm.h>

const GLuint SCREEN_WIDTH = 800;
const GLuint SCREEN_HEIGHT = 600;

void loadShader(const GLchar* filePath, GLchar** shader);
void compileShaders(const GLchar* vertexPath, const GLchar* fragmentPath, GLuint* shaderID);
void checkCompileErrors(GLuint object, const char* type);

// Shaders
GLuint shaderID, screenShaderID;
GLuint fbo, textureBuffer;
char* vertexShader, fragmentShader;

// Mouse
float lastX;
float lastY;
bool firstMouse;

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main()
{
  // Init context
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  #endif

  GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Minimal", NULL, NULL);
  if (!window)
  {
    printf("Failed to create GLFW window");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    printf("Failed to initialize GLAD\n");
    return -1;
  }

  // Init quad
  GLuint VBO, VAO;
  GLfloat vertices[] = {
    // positions         // colors
    0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
    0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top
  };

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(VAO);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);

  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Compile shaders
  compileShaders("triangle.vs", "triangle.fs", &shaderID);
  glUseProgram(shaderID);

  int macMoved = 0;
  while (!glfwWindowShouldClose(window))
  {
    // render
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // render the triangle
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // swap buffers and poll events
    glfwSwapBuffers(window);
    glfwPollEvents();
    #ifdef __APPLE__
        if (macMoved == 0)
        {
          int x, y;
          glfwGetWindowPos(window, &x, &y);
          glfwSetWindowPos(window, ++x, y);
          macMoved = 1;
        }
    #endif
  }

  glfwTerminate();

  return 0;
}

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
