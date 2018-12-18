#include "../engine/steve.h"

#include "ui.h"

const GLuint SCREEN_WIDTH = 800;
const GLuint SCREEN_HEIGHT = 600;

// Shaders
GLuint shader_id;
char* vertexShader, fragmentShader;

float delta_time = 0.0f; // time between current frame and last frame
float last_frame = 0.0f;
float camera_speed = 50.0f;

// Camera
camera cam = {
  .pos = {0.0f, 0.0f, 0.3f},
  .up = {0.0f, 1.0f, 0.0f},
  .front = {0.0f, 0.0f, -1.0f}
};

// Mouse
int first_mouse = 1;
float yaw = -90.0f;
float pitch = 0.0f;
float mouse_last_x = SCREEN_WIDTH / 2.0;
float mouse_last_y = SCREEN_HEIGHT / 2.0;
float fov = 45.0f;
float sensitivity = 0.01f; // change this value to your liking

// Track
object* objects[4];

static void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
  float camera_delta = camera_speed * delta_time;
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    vec3 vec3_scaled;
    vec3_scale(vec3_scaled, cam.front, camera_delta);
    vec3_add(cam.pos, cam.pos, vec3_scaled);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    vec3 vec3_temp;
    vec3_scale(vec3_temp, cam.front, camera_delta);
    vec3_sub(cam.pos, cam.pos, vec3_temp);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    vec3 vec3_crossed, vec3_scaled, vec3_normalized;
    vec3_mul_cross(vec3_crossed, cam.front, cam.up);
    vec3_norm(vec3_normalized, vec3_crossed);
    vec3_scale(vec3_scaled, vec3_normalized, camera_delta);
    vec3_sub(cam.pos, cam.pos, vec3_scaled);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    vec3 vec3_crossed, vec3_scaled, vec3_normalized;
    vec3_mul_cross(vec3_crossed, cam.front, cam.up);
    vec3_norm(vec3_normalized, vec3_crossed);
    vec3_scale(vec3_scaled, vec3_normalized, camera_delta);
    vec3_add(cam.pos, cam.pos, vec3_scaled);
  }
}

static void mouse_callback(GLFWwindow *window, double x_pos, double y_pos)
{
  if (first_mouse == 1) {
    mouse_last_x = x_pos;
    mouse_last_y = y_pos;
    first_mouse = false;
  }

  float x_offset = x_pos - mouse_last_x;
  float y_offset = mouse_last_y - y_pos; // reversed since y-coordinates go from bottom to top
  mouse_last_x = x_pos;
  mouse_last_y = y_pos;

  x_offset *= sensitivity;
  y_offset *= sensitivity;

  yaw += x_offset;
  pitch += y_offset;

  // make sure that when pitch is out of bounds, screen doesn't get flipped
  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;

  cam.front[0] = cosf(yaw) * cosf(pitch);
  cam.front[1] = sinf(pitch);
  cam.front[2] = sinf(yaw) * cosf(pitch);
}

object* load_asset(const char* filename) {
  object* obj = malloc(sizeof(object));
  mesh* meshes;
  int num_meshes;
  importer_load_obj(filename, &meshes, &num_meshes);
  obj->meshes = meshes;
  obj->num_meshes = num_meshes;
  quat_identity(obj->rotation);
  obj->scale = 1.0f;
  return obj;
}

void init_track() {
  objects[0] = load_asset("assets/racing/track.obj");
  vec3 pos_1 = {0.0f, -2.0f, -9.0f};
  vec3_copy(objects[0]->position, pos_1);
  renderer_add_object(objects[0]);

  objects[1] = load_asset("assets/racing/raceCarRed.obj");
  vec3 pos_2 = {-0.5f, -2.0f, -9.2f};
  vec3_copy(objects[1]->position, pos_2);
  objects[1]->scale = 0.25f;
  renderer_add_object(objects[1]);
}

int main()
{
  // Init context
  GLFWwindow* window;
  if (renderer_init("Microdrag", SCREEN_WIDTH, SCREEN_HEIGHT, key_callback, mouse_callback, &window) < 0) {
    printf("Error initializing renderer!\n");
    return -1;
  }

  // import obj
  init_track();

  // Compile shaders
  shader_compile("shaders/lighting.vs", "shaders/lighting.fs", &shader_id);

  // init ui
  ui_init(window);

  /* lights */
  light* lights[1];
  light l1;
  l1.position[0] = 0.0f;
  l1.position[1] = 1.0f;
  l1.position[2] = 0.0f;
  l1.color[0] = 1.0f;
  l1.color[1] = 1.0f;
  l1.color[2] = 1.0f;

  lights[0] = &l1;

  int macMoved = 0;
  while (!renderer_should_close()) {
    // per-frame time logic
    // --------------------
    float current_frame = glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    // render
    vec3 y_axis = {0.0f, 1.0f, 0.0f};
    // quat_rotate(t1.rotation, (float)glfwGetTime(), y_axis);
    // t1.scale = 1.0f + sinf(t1.scale + (float)glfwGetTime());
    // t1.position[1] = sinf((float)glfwGetTime());
    // quat_rotate(t2.rotation, (float)glfwGetTime(), z_axis);

    renderer_render_objects(objects, 2, lights, 1, shader_id, &cam, ui_render);
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
  ui_free();
  renderer_cleanup();
  free(objects[0]->meshes);

  return 0;
}
