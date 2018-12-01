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

  object* objects[1];

  object t1;
  face faces[model_data.num_faces];
  GLuint texcoords[model_data.num_texcoords];

  t1.position[0] = 0.0f;
  t1.position[1] = 0.0f;
  t1.position[2] = -9.0f;

  t1.num_vertices = model_data.num_vertices;

  GLuint* indices = malloc(model_data.num_faces * sizeof(GLuint));
  vertex* vertices = malloc(model_data.num_vertices * sizeof(vertex));
  int j = 0;

  int k = 0;
  for (int i = 0; i < model_data.num_vertices; i++) {
    vertices[i].x = model_data.vertices[k];
    vertices[i].y = model_data.vertices[k + 1];
    vertices[i].z = model_data.vertices[k + 2];
    k = k + 3;
  }

  for (int i = 0; i < model_data.num_faces; i++)
  {
    indices[i] = model_data.faces[i].v_idx;

    GLuint vt1 = model_data.faces[i].vt_idx;

    // printf("TEXTCOORD %u %u %u\n", vt1, vt2, vt3);

        // printf("INDEX[%d]: %d %d %d\n", i, indices[i], indices[i + 1], indices[i + 2]);

        /* vertex v1 = parsed_vertices[indices[i]];
    vertex v2 = parsed_vertices[indices[i + 1]];
    vertex v3 = parsed_vertices[indices[i + 2]]; */

        /*GLfloat vx = o->vertices[indices[k]];
    GLfloat vy = o->vertices[indices[k + 1]];
    GLfloat vz = o->vertices[indices[k + 2]]; */

        /* vertices[i] = v1;
    vertices[i + 1] = v2;
    vertices[i + 2] = v3; */

        /* vertices[j] = v1;
    vertices[j + 1] = v2;
    vertices[j + 2] = v3;
    vertices[j + 3] = 0;
    vertices[j + 4] = 0;
    vertices[j + 5] = 0;
    vertices[j + 6] = 0;
    vertices[j + 7] = 0; */

    /*     GLfloat vt1 = o->texcoords[o->faces[i].vt_idx];
    GLfloat vt2 = o->texcoords[o->faces[i + 1].vt_idx];

    GLfloat vn1 = o->texcoords[o->faces[i].vn_idx];
    GLfloat vn2 = o->texcoords[o->faces[i + 1].vn_idx];
    GLfloat vn3 = o->texcoords[o->faces[i + 2].vn_idx];*/

    //printf("VERTEX: %f %f %f\n", v1, v2, v3);
    // printf("first element %d\n", indices[0]);
  }

  /* for (int i = 0; i < model_data.num_faces; i++) {
    printf("i: %u %u\n", i, indices[i]);
  } */

  t1.vertices = vertices;

  // create the array for the faces
  for (int i = 0; i < model_data.num_faces; i++) {
    face f = {
      model_data.faces[i].v_idx,
      model_data.faces[i].vt_idx,
      model_data.faces[i].vn_idx
    };
    faces[i] = f;
  }
  t1.indices = indices;
  t1.num_faces = model_data.num_faces;

  t1.texcoords = model_data.texcoords;

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

  renderer_cleanup();

  return 0;
}
