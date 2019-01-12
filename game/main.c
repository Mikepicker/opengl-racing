#include "../engine/steve.h"

#include "ui.h"
#include "editor.h"

const GLuint SCREEN_WIDTH = 1200;
const GLuint SCREEN_HEIGHT = 800;

float delta_time = 0.0f; // time between current frame and last frame
float last_frame = 0.0f;
float camera_speed = 50.0f;

// Camera
camera cam = {
  .pos = {0.0f, 2.0f, 9.0f},
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

// game objects
#define GAME_OBJECTS_COUNT 1
object* game_objects[GAME_OBJECTS_COUNT];

// render list
render_list* rl;

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

  // editor
  if (action == GLFW_RELEASE) {
    vec3 pos = { 0.0f, 0.0f, 0.0f };
    if (key == GLFW_KEY_E) {
      editor_next_piece();
    } else if (key == GLFW_KEY_Q) {
      editor_rotate_piece();
    } else if (key == GLFW_KEY_LEFT) {
      pos[0] = -1.0f;
    } else if (key == GLFW_KEY_RIGHT) {
      pos[0] = 1.0f;
    } else if (key == GLFW_KEY_UP) {
      pos[2] = -1.0f;
    } else if (key == GLFW_KEY_DOWN) {
      pos[2] = 1.0f;
    } else if (key == GLFW_KEY_ENTER) {
      editor_place_piece();
    } else if (key == GLFW_KEY_R) {
      editor_remove_piece();
    }
    editor_move_piece(pos);
  }

  // capture mouse
  if (action == GLFW_RELEASE) {
    if (key == GLFW_KEY_P) {
      renderer_capture_mouse(!(glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL));
    }
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

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    renderer_capture_mouse(!(glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL));
}

void init_track() {
  // objects[0] = importer_load_obj("assets/racing/track.obj");
  // vec3 pos_1 = {0.0f, -2.0f, -9.0f};
  // vec3_copy(objects[0]->position, pos_1);
  // renderer_add_object(objects[0]);

  game_objects[0] = importer_load_obj("assets/racing/raceCarRed.obj");
  vec3 pos_2 = {0.0f, 0.0f, 0.0f};
  vec3_copy(game_objects[0]->position, pos_2);
  // game_objects[0]->scale = 0.25f;
  game_objects[0]->box = physics_compute_aabb(game_objects[0]);
  renderer_add_object(game_objects[0]);
}

int main()
{
  // Init context
  GLFWwindow* window;
  if (renderer_init("Microdrag", SCREEN_WIDTH, SCREEN_HEIGHT, key_callback, mouse_callback, mouse_button_callback, &window) < 0) {
    printf("Error initializing renderer!\n");
    return -1;
  }

  // import obj
  init_track();

  // init ui
  ui_init(window);

  // init editor
  editor_init();

  // render list
  rl = render_list_new();

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

    // renderer_render_objects(objects, 2, lights, 1, shader_id, &cam, NULL);
    
    // test road collision
    for (int i = 0; i < EDITOR_MAX_PLACED_OBJECTS; i++) {
      object* o = editor_placed_objects[i];
      if (o != NULL) {
        // vec3 p = { game_objects[0]->box.max_x, game_objects[0]->box.max_y, game_objects[0]->box.max_z };
        vec3 p = { -0.1f, 10.0f, 0 };
        ray ray_test;
        vec3 ray_dir = { 0, -1.0f, 0 };
        vec3_copy(ray_test.o, p);
        vec3_copy(ray_test.dir, ray_dir);
        mesh* m = physics_ray_hit_mesh(ray_test, o);
        if (m != NULL) {
          printf("COLLIDE MESH %s\n", m->mat.name);
        }
      }
    }

    render_list_clear(rl);
    render_list_add(rl, game_objects[0]);
    render_list_add_batch(rl, editor_render_list, editor_render_list_size);

    // ui
    ui_set_camera(cam);

    // render editor
    renderer_render_objects(rl->objects, rl->size, lights, 1, &cam, ui_render, ui_debug);

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
  render_list_free(rl);
  editor_free();

  return 0;
}
