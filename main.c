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

  // import obj
  mesh* meshes;
  int num_meshes;
  importer_load_obj("assets/tower.obj", &meshes, &num_meshes);

  // Compile shaders
  shader_compile("shaders/lighting.vs", "shaders/lighting.fs", &shader_id);

  /* objects */

  // init object list
  object* objects[1];
  object t1;

  // set position
  t1.position[0] = 0.0f;
  t1.position[1] = -2.0f;
  t1.position[2] = -9.0f;

  // set scale
  t1.scale = 1.0f;

  // set geometry data
  t1.meshes = meshes;
  t1.num_meshes = num_meshes;
  
  // init rotation quaternion
  quat_identity(t1.rotation);

  // add the object to the renderer
  renderer_add_object(&t1);

  // add object to objects list
  objects[0] = &t1;

  /* lights */
  light* lights[2];
  light l1;
  l1.position[0] = 0.0f;
  l1.position[1] = 1.0f;
  l1.position[2] = 0.0f;
  l1.color[0] = 1.0f;
  l1.color[1] = 0.0f;
  l1.color[2] = 0.0f;

  light l2;
  l2.position[0] = 0.0f;
  l2.position[1] = 1.0f;
  l2.position[2] = 0.0f;
  l2.color[0] = 0.0f;
  l2.color[1] = 0.0f;
  l2.color[2] = 1.0f;

  lights[0] = &l1;
  lights[1] = &l2;

  int macMoved = 0;
  while (!renderer_should_close()) {

    // render
    vec3 y_axis = {0.0f, 1.0f, 0.0f};
    quat_rotate(t1.rotation, (float)glfwGetTime(), y_axis);
    // t1.scale = 1.0f + sinf(t1.scale + (float)glfwGetTime());
    // t1.position[1] = sinf((float)glfwGetTime());
    // quat_rotate(t2.rotation, (float)glfwGetTime(), z_axis);

    l1.position[0] = cosf((float)glfwGetTime() * 10);
    l2.position[0] = 100 * sinf((float)glfwGetTime() * 10);
    renderer_render_objects(objects, 1, lights, 2, shader_id);
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

  // cleanup
  renderer_cleanup();
  for (int i = 0; i < num_meshes; i++) {
    mesh_free(&meshes[i]);
  }
  free(meshes);

  return 0;
}
