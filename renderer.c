#include "renderer.h"

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

void renderer_render_objects(object* objects, GLFWwindow* window, GLuint shaderID) {
  GLint mvp_location;
  float ratio;
  int width, height;
  mat4x4 m, p, mvp;

  mvp_location = glGetUniformLocation(shaderID, "MVP");
  glUseProgram(shaderID);

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glfwGetFramebufferSize(window, &width, &height);
  ratio = width / (float)height;
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT);
  mat4x4_identity(m);
  mat4x4_rotate_Z(m, m, (float)glfwGetTime());
  mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
  mat4x4_mul(mvp, p, m);
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);

  // render the triangle
  glBindVertexArray(objects->vao);
  glDrawArrays(GL_TRIANGLES, 0, 3);

  // swap buffers and poll events
  glfwSwapBuffers(window);
}
