#include <math.h>

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
  if (!renderer_init("Microdrag", SCREEN_WIDTH, SCREEN_HEIGHT, key_callback)) {
    printf("Error initializing renderer!\n");
    return -1;
  }

  // Init quad
  GLfloat vertices[] = {
    // positions         // colors
    0.6f, -0.4f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
    -0.6f, -0.4f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
    0.0f,  0.6f, 0.0f,  0.0f, 0.0f, 1.0f   // top
  };

  object* objects[2];

  object t1;
  object t2;

  t1.position[0] = 0.0f;
  t1.position[1] = 0.0f;
  t1.position[2] = 0.0f;
  t1.vertices = vertices;
  t1.num_vertices = sizeof(vertices) / sizeof(GLfloat);
  quat_identity(t1.rotation);
  renderer_add_object(&t1);

  t2.position[0] = 1.0f;
  t2.position[1] = 0.0f;
  t2.position[2] = 0.0f;
  t2.vertices = vertices;
  t2.num_vertices = sizeof(vertices) / sizeof(GLfloat);
  quat_identity(t2.rotation);
  renderer_add_object(&t2);

  objects[0] = &t1;
  objects[1] = &t2;

  // Compile shaders
  shader_compile("triangle.vs", "triangle.fs", &shader_id);

  int macMoved = 0;
  while (!renderer_should_close()) {

    // render
    vec3 z_axis = {0.0f, 0.0f, 1.0f};
    quat_rotate(t1.rotation, (float)glfwGetTime(), z_axis);
    t2.position[0] = sinf((float)glfwGetTime());
    quat_rotate(t2.rotation, (float)glfwGetTime(), z_axis);

    renderer_render_objects(objects, 2, window, shader_id);
  }

  renderer_cleanup();

  return 0;
}
