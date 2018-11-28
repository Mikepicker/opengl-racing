#include "steve.h"

const GLuint SCREEN_WIDTH = 800;
const GLuint SCREEN_HEIGHT = 600;

// Shaders
GLuint shader_id;
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

  GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Microdrag", NULL, NULL);
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
  GLfloat vertices[] = {
    // positions         // colors
    0.6f, -0.4f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
    -0.6f, -0.4f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
    0.0f,  0.6f, 0.0f,  0.0f, 0.0f, 1.0f   // top
  };

  object triangle;
  triangle.vertices = vertices;
  triangle.num_vertices = sizeof(vertices) / sizeof(GLfloat);
  renderer_add_object(&triangle);

  // Compile shaders
  shader_compile("triangle.vs", "triangle.fs", &shader_id);

  int macMoved = 0;
  while (!glfwWindowShouldClose(window)) {

    // render
    renderer_render_objects(&triangle, window, shader_id);
    
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
