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
  if (renderer_init("Microdrag", SCREEN_WIDTH, SCREEN_HEIGHT, key_callback) < 0) {
    printf("Error initializing renderer!\n");
    return -1;
  }

  // Compile shaders
  shader_compile("triangle.vs", "triangle.fs", &shader_id);

  // import obj
  tinyobj_attrib_t model_data;
  importer_load_obj("./assets/crate.obj", &model_data);

  // Init quad
  /*GLfloat vertices[] = {
    // positions         // colors
    0.6f, -0.4f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
    -0.6f, -0.4f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
    0.0f,  0.6f, 0.0f,  0.0f, 0.0f, 1.0f   // top
  };*/

  /* GLfloat vertices[] = {
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
      0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

      -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f}; */

  object* objects[1];

  object t1;
  // object t2;

  // float vertices = model_data.vertices[5];

  t1.position[0] = 0.0f;
  t1.position[1] = 0.0f;
  t1.position[2] = -9.0f;
  t1.vertices = model_data.vertices;
  GLuint faces[33];
  for (int i = 0; i < model_data.num_faces; i++)
  {
    // printf("FACE: %d\n", model_data.faces[i].v_idx);
    faces[i] = model_data.faces[i].v_idx;
  }
  t1.faces = faces;
  t1.num_vertices = model_data.num_vertices;
  t1.num_faces = model_data.num_faces;
  printf("Num vertices %d\n", t1.num_vertices);
  quat_identity(t1.rotation);
  renderer_add_object(&t1);

  /* t2.position[0] = 1.0f;
  t2.position[1] = 0.0f;
  t2.position[2] = 0.0f;
  t2.vertices = vertices;
  t2.num_vertices = sizeof(vertices) / sizeof(GLfloat);
  quat_identity(t2.rotation);
  renderer_add_object(&t2); */

  objects[0] = &t1;
  // objects[1] = &t2;


  int macMoved = 0;
  while (!renderer_should_close()) {

    // render
    vec3 y_axis = {0.0f, 1.0f, 0.0f};
    quat_rotate(t1.rotation, (float)glfwGetTime(), y_axis);
    // t1.position[1] = sinf((float)glfwGetTime());
    // quat_rotate(t2.rotation, (float)glfwGetTime(), z_axis);

    renderer_render_objects(objects, 1, window, shader_id);
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

  renderer_cleanup();

  return 0;
}
