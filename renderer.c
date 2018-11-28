#include "renderer.h"
#include "debug.h"

int renderer_init(char* title, int width, int height, void* key_callback) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  #endif

  window = glfwCreateWindow(width, height, title, NULL, NULL);
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
}

void renderer_cleanup() {
  glfwTerminate();
}

int renderer_should_close() {
  return glfwWindowShouldClose(window);
}

void renderer_add_object(object* o) {
  glGenVertexArrays(1, &(o->vao)); // Vertex Array Object
  glGenBuffers(1, &(o->vbo)); // Vertex Buffer Object

  glBindBuffer(GL_ARRAY_BUFFER, o->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * o->num_vertices, o->vertices, GL_STATIC_DRAW);

  glBindVertexArray(o->vao);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);

  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void renderer_render_objects(object* objects[], int objects_length, GLFWwindow* window, GLuint shader_id) {
  GLint mvp_location;
  float ratio;
  int width, height;

  mvp_location = glGetUniformLocation(shader_id, "MVP");
  glUseProgram(shader_id);

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glfwGetFramebufferSize(window, &width, &height);
  ratio = width / (float)height;
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT);

  for (int i = 0; i < objects_length; i++) {
    object* o = objects[i];
    mat4x4 m, p, mvp;

    // translate
    mat4x4_translate(m, o->position[0], o->position[1], o->position[2]);

    // compute rotation matrix from quaternion
    mat4x4 mat_rot;
    mat4x4_from_quat(mat_rot, o->rotation);

    // rotate
    mat4x4_mul(m, m, mat_rot);

    // compute mvp matrix
    mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    mat4x4_mul(mvp, p, m);

    // pass mvp to shader
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);

    // render the triangle
    glBindVertexArray(o->vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
  }

  // swap buffers and poll events
  glfwSwapBuffers(window);

  // poll events
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
