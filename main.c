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
  mesh* meshes;
  int num_meshes;
  importer_load_obj("./assets/tower.obj", &meshes, &num_meshes);

  // init object list
  object* objects[1];
  object t1;

  // set position
  t1.position[0] = 0.0f;
  t1.position[1] = -2.0f;
  t1.position[2] = -9.0f;

  // set geometry data
  t1.meshes = meshes;
  t1.num_meshes = num_meshes;
  
  // init rotation quaternion
  quat_identity(t1.rotation);

  // add the object to the renderer
  renderer_add_object(&t1);

  objects[0] = &t1;

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

  // cleanup
  renderer_cleanup();
  for (int i = 0; i < num_meshes; i++) {
    mesh_free(&meshes[i]);
  }
  free(meshes);

  return 0;
}
