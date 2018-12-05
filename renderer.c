#define STB_IMAGE_IMPLEMENTATION
#include "renderer.h"

unsigned int load_image(char* filename) {
  unsigned int texture = -1;

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load image, create texture and generate mipmaps
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
  // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
  unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
  if (data) {
    GLenum format;
    if (nrChannels == 1)
      format = GL_RED;
    if (nrChannels == 2)
      format = GL_ALPHA;
    else if (nrChannels == 3)
      format = GL_RGB;
    else if (nrChannels == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
  else {
    printf("Error loading texture\n");
  }
  stbi_image_free(data);
  return texture;
}

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
  glGenBuffers(1, &(o->ebo));      // Element Buffer Object

  glBindVertexArray(o->vao);

  glBindBuffer(GL_ARRAY_BUFFER, o->vbo);
  glBufferData(GL_ARRAY_BUFFER, o->num_vertices * sizeof(vertex), o->vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, o->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, o->num_indices * sizeof(GLuint), o->indices, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
  glEnableVertexAttribArray(0);

  // texture coord attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  // normals attribute
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(5 * sizeof(GLfloat)));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // TODO: properly set texture
  o->texture_id = load_image("assets/crate.png");
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

    // bind texture
    glUniform1i(glGetUniformLocation(o->texture_id, "texture1"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, o->texture_id);

    // render the triangle
    glBindVertexArray(o->vao);
    glDrawElements(GL_TRIANGLES, o->num_indices, GL_UNSIGNED_INT , 0);
  }

  // swap buffers and poll events
  glfwSwapBuffers(window);

  // poll events
  glfwPollEvents();
}
