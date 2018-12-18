#define STB_IMAGE_IMPLEMENTATION
#include "renderer.h"

void set_opengl_state() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
}

unsigned int load_image(char* filename) {
  GLuint texture = -1;

  if (strlen(filename) == 0) {
    return 0;
  }

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
    printf("Error loading texture: %s\n", filename);
  }

  stbi_image_free(data);
  return texture;
}

int renderer_init(char* title, int width, int height, void* key_callback, void* mouse_callback, GLFWwindow** out_window) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  window = glfwCreateWindow(width, height, title, NULL, NULL);
  *out_window = window;
  if (!window)
  {
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    printf("Failed to initialize GLAD\n");
    return -1;
  }

  // set opengl state
  set_opengl_state();

  return 0;
}

void renderer_cleanup() {
  glfwTerminate();
}

int renderer_should_close() {
  return glfwWindowShouldClose(window);
}

void renderer_add_object(object* o) {

  for (int i = 0; i < o->num_meshes; i++) {
    mesh* mesh = &o->meshes[i];
    glGenVertexArrays(1, &(mesh->vao)); // Vertex Array Object
    glGenBuffers(1, &(mesh->vbo));      // Vertex Buffer Object
    glGenBuffers(1, &(mesh->ebo));      // Element Buffer Object

    glBindVertexArray(mesh->vao);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh->num_vertices * sizeof(vertex), mesh->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->num_indices * sizeof(GLuint), mesh->indices, GL_STATIC_DRAW);

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

    // texture
    mesh->texture_id = load_image(mesh->mat.texture_path);
  }
}

void renderer_render_objects(object *objects[], int objects_length, light *lights[], int lights_length, GLuint shader_id, camera *camera, void (*ui_render_callback)(void))
{
  GLint m_location, v_location, p_location, time;
  GLint uniform_diffuse, uniform_specular;
  float ratio;
  int width, height;

  glUseProgram(shader_id);

  // set bg color
  glClearColor(183.0f / 255.0f, 220.0f / 255.0f, 244.0f / 255.0f, 1.0f);

  // handle window resize
  glfwGetFramebufferSize(window, &width, &height);
  ratio = width / (float)height;
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // matrices uniforms
  m_location = glGetUniformLocation(shader_id, "M");
  v_location = glGetUniformLocation(shader_id, "V");
  p_location = glGetUniformLocation(shader_id, "P");

  // time uniform
  time = glGetUniformLocation(shader_id, "time");

  // material uniforms
  uniform_diffuse = glGetUniformLocation(shader_id, "material.diffuse");
  uniform_specular = glGetUniformLocation(shader_id, "material.specular");

  // camera position
  vec3 camera_pos = {0.0f, 0.0f, 0.0f};
  GLint uniform_camera_pos = glGetUniformLocation(shader_id, "cameraPos");
  glUniform3fv(uniform_camera_pos, 1, (const GLfloat*) camera_pos);

  // process lights
  glUniform1i(glGetUniformLocation(shader_id, "lightsNr"), lights_length);
  for (int i = 0; i < lights_length; i++) {
    char uniform_light_pos[256];
    sprintf(uniform_light_pos, "lightsPos[%d]", i);
    char uniform_light_color[256];
    sprintf(uniform_light_color, "lightsColors[%d]", i);
    glUniform3fv(glGetUniformLocation(shader_id, uniform_light_pos), 1, (const GLfloat*) lights[i]->position);
    glUniform3fv(glGetUniformLocation(shader_id, uniform_light_color), 1, (const GLfloat*) lights[i]->color);
  }

  // process objects
  for (int i = 0; i < objects_length; i++) {
    object* o = objects[i];
    mat4x4 m, p, v;

    // scale
    mat4x4_identity(m);
    mat4x4_scale(m, m, o->scale);

    // translate
    mat4x4_translate(m, o->position[0], o->position[1], o->position[2]);

    // compute rotation matrix from quaternion
    mat4x4 mat_rot;
    mat4x4_from_quat(mat_rot, o->rotation);

    // rotate
    mat4x4_mul(m, m, mat_rot);

    // mat4x4_identity(v);

    // compute mvp matrix
    // mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    vec3 cameraDirection;
    vec3_add(cameraDirection, camera->pos, camera->front);
    // printf("Camera Pos: (%f, %f, %f)\n", camera->pos[0], camera->pos[1], camera->pos[2]);
    // printf("Camera Front: (%f, %f, %f)\n", camera->front[0], camera->front[1], camera->front[2]);
    mat4x4_look_at(v, camera->pos, cameraDirection, camera->up);
    mat4x4_perspective(p, to_radians(45.0f), ratio, 0.1f, 100.0f);

    // pass mvp to shader
    glUniformMatrix4fv(m_location, 1, GL_FALSE, (const GLfloat*) m);
    glUniformMatrix4fv(v_location, 1, GL_FALSE, (const GLfloat*) v);
    glUniformMatrix4fv(p_location, 1, GL_FALSE, (const GLfloat*) p);

    // pass time to shader
    glUniform1f(time, (float)glfwGetTime());

    for (int i = 0; i < o->num_meshes; i++) {
      mesh* mesh = &o->meshes[i];

      // pass material
      glUniform3fv(uniform_diffuse, 1, mesh->mat.diffuse);
      glUniform3fv(uniform_specular, 1, mesh->mat.specular);

      // bind texture
      if (strlen(mesh->mat.texture_path) > 0) {
        glUniform1i(glGetUniformLocation(mesh->texture_id, "texture1"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mesh->texture_id);
        glUniform1i(glGetUniformLocation(shader_id, "hasTexture"), 1);
      } else {
        glUniform1i(glGetUniformLocation(shader_id, "hasTexture"), 0);
      }

      // render the triangle
      glBindVertexArray(mesh->vao);
      glDrawElements(GL_TRIANGLES, mesh->num_indices, GL_UNSIGNED_INT , 0);
    }
  }

  // ui callback
  ui_render_callback();

  // reset opengl state
  set_opengl_state();

  // swap buffers and poll events
  glfwSwapBuffers(window);

  // poll events
  glfwPollEvents();
}
