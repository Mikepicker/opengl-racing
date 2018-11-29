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
    printf("Failed to create GLFW window\n");
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
  glEnable(GL_DEPTH_TEST);
  return 0;
}

void renderer_cleanup() {
  glfwTerminate();
}

int renderer_should_close() {
  return glfwWindowShouldClose(window);
}

void renderer_add_object(object* o) {
  glGenVertexArrays(1, &(o->vao)); // Vertex Array Object
  glGenBuffers(1, &(o->vbo));      // Vertex Buffer Object
  glGenBuffers(1, &(o->ebo));           // Element Buffer Object

  glBindVertexArray(o->vao);

  glBindBuffer(GL_ARRAY_BUFFER, o->vbo);
  glBufferData(GL_ARRAY_BUFFER, o->num_vertices * 3 * sizeof(GLfloat), o->vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, o->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, o->num_faces * sizeof(GLuint), o->faces, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);

  // color attribute
  /*glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);*/

  // texture coord attribute
  // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
  // glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void renderer_render_objects(object* objects[], int objects_length, GLFWwindow* window, GLuint shader_id) {
  GLint m_location, v_location, p_location;
  float ratio;
  int width, height;

  m_location = glGetUniformLocation(shader_id, "M");
  v_location = glGetUniformLocation(shader_id, "V");
  p_location = glGetUniformLocation(shader_id, "P");
  glUseProgram(shader_id);

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

  glfwGetFramebufferSize(window, &width, &height);
  ratio = width / (float)height;
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (int i = 0; i < objects_length; i++) {
    object* o = objects[i];
    mat4x4 m, p, v;

    // translate
    mat4x4_translate(m, o->position[0], o->position[1], o->position[2]);

    // compute rotation matrix from quaternion
    mat4x4 mat_rot;
    mat4x4_from_quat(mat_rot, o->rotation);

    // rotate
    mat4x4_mul(m, m, mat_rot);

    mat4x4_identity(v);

    // compute mvp matrix
    // mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    vec3 eye = {0.0f, 0.0f, 0.0f};
    vec3 center = {0.0f, 0.0f, -1.0f};
    vec3 up = {0.0f, 1.0f, 0.0f};
    // mat4x4_look_at(v, eye, center, up);
    mat4x4_perspective(p, to_radians(45.0f), ratio, 0.1f, 100.0f);

    // pass mvp to shader
    glUniformMatrix4fv(m_location, 1, GL_FALSE, (const GLfloat*) m);
    glUniformMatrix4fv(v_location, 1, GL_FALSE, (const GLfloat*) v);
    glUniformMatrix4fv(p_location, 1, GL_FALSE, (const GLfloat*) p);

    // render the triangle
    glBindVertexArray(o->vao);
    glDrawElements(GL_TRIANGLES, o->num_faces, GL_UNSIGNED_INT , 0);
  }

  // swap buffers and poll events
  glfwSwapBuffers(window);

  // poll events
  glfwPollEvents();
}
