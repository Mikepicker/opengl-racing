#include "steve.h"

const GLuint SCREEN_WIDTH = 800;
const GLuint SCREEN_HEIGHT = 600;

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
  GLint mvp_location;
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
    0.6f, -0.4f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
    -0.6f, -0.4f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
    0.0f,  0.6f, 0.0f,  0.0f, 0.0f, 1.0f   // top
  };

  glGenVertexArrays(1, &VAO); // Vertex Array Object
  glGenBuffers(1, &VBO); // Vertex Buffer Object

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
  mvp_location = glGetUniformLocation(shaderID, "MVP");
  glUseProgram(shaderID);

  int macMoved = 0;
  float ratio;
  int width, height;
  mat4x4 m, p, mvp;
  while (!glfwWindowShouldClose(window))
  {
    // render
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    mat4x4_identity(m);
    mat4x4_rotate_Z(m, m, (float)glfwGetTime());
    mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    mat4x4_mul(mvp, p, m);
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);

    // render the triangle
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // swap buffers and poll events
    glfwSwapBuffers(window);
    glfwPollEvents();
    #ifdef __APPLE__ // TODO: remove this workaround with glfw 3.3
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
