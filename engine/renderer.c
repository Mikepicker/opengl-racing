#define STB_IMAGE_IMPLEMENTATION
#include "renderer.h"

void set_opengl_state() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_CULL_FACE);
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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

static void init_depth_fbo() {
  glGenFramebuffers(1, &renderer_depth_fbo);
  // create depth texture
  glGenTextures(1, &renderer_depth_map);
  glBindTexture(GL_TEXTURE_2D, renderer_depth_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  // attach depth texture as FBO's depth buffer
  glBindFramebuffer(GL_FRAMEBUFFER, renderer_depth_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderer_depth_map, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int renderer_init(char* title, int width, int height, void* key_callback, void* mouse_callback, void* mouse_button_callback, GLFWwindow** out_window) {
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
  if (!window) {
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    return -1;
  }

  // init vars
  renderer_debug_vao = 0;
  renderer_debug_enabled = 0;
  renderer_shadow_bias = 0.22f;
  renderer_shadow_pcf_enabled = 0;

  // init depth fbo
  init_depth_fbo();

  // set opengl state
  set_opengl_state();

  // compile shaders
  renderer_recompile_shader();

  return 0;
}

void renderer_cleanup() {
  glfwTerminate();
}

void renderer_recompile_shader() {
  shader_compile("../engine/shaders/toon.vs", "../engine/shaders/toon.fs", &renderer_main_shader);
  shader_compile("../engine/shaders/shadow.vs", "../engine/shaders/shadow.fs", &renderer_shadow_shader);
  shader_compile("../engine/shaders/debug.vs", "../engine/shaders/debug.fs", &renderer_debug_shader);
}

int renderer_should_close() {
  return glfwWindowShouldClose(window);
}

static void add_aabb(object* o) {
  aabb* aabb = &o->box;
  GLfloat vertices[] = {
    -0.5, -0.5, -0.5,
     0.5, -0.5, -0.5,
     0.5,  0.5, -0.5,
    -0.5,  0.5, -0.5,
    -0.5, -0.5,  0.5,
     0.5, -0.5,  0.5,
     0.5,  0.5,  0.5,
    -0.5,  0.5,  0.5,
  };

  glGenVertexArrays(1, &(aabb->vao)); // Vertex Array Object
  glGenBuffers(1, &(aabb->vbo));      // Vertex Buffer Object
  glGenBuffers(1, &(aabb->ebo));      // Element Buffer Object

  glBindVertexArray(aabb->vao);

  glBindBuffer(GL_ARRAY_BUFFER, aabb->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLushort elements[] = {
    0, 1, 2, 3,
    4, 5, 6, 7,
    0, 4, 1, 5, 2, 6, 3, 7
  };
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, aabb->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
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

    add_aabb(o);
  }
}

static void render_aabb(object* o) {
  aabb* aabb = &o->box;
  mat4x4 m;
  mat4x4_identity(m);

  vec3 size = {aabb->max_x - aabb->min_x, aabb->max_y - aabb->min_y, aabb->max_z - aabb->min_z};
  mat4x4_scale_aniso(m, m, size[0], size[1], size[2]);

  vec3 center = {
    (aabb->min_x + aabb->max_x) / 2,
    (aabb->min_y + aabb->max_y) / 2,
    (aabb->min_z + aabb->max_z) / 2
  };

  mat4x4 translation;
  vec3 pos = {
    (o->position[0] * o->scale + center[0]) / size[0],
    (o->position[1] * o->scale + center[1]) / size[1],
    (o->position[2] * o->scale + center[2]) / size[2]
  };

  mat4x4_translate(translation, pos[0], pos[1], pos[2]);
  mat4x4_mul(m, m, translation);

  glUniformMatrix4fv(glGetUniformLocation(renderer_main_shader, "M"), 1, GL_FALSE, (const GLfloat*) m);

  glBindVertexArray(aabb->vao);
  glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
  glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
  glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void render_objects(object *objects[], int objects_length, GLuint shader_id, int debug) {
  for (int i = 0; i < objects_length; i++) {
    object* o = objects[i];
    mat4x4 m;

    // scale
    mat4x4_identity(m);
    mat4x4_scale(m, m, o->scale);

    // translate
    mat4x4 translation;
    mat4x4_translate(translation, o->position[0], o->position[1], o->position[2]);
    mat4x4_mul(m, m, translation);

    // compute rotation matrix from quaternion
    mat4x4 mat_rot;
    mat4x4_from_quat(mat_rot, o->rotation);

    // rotate around center
    mat4x4 t1;
    mat4x4_translate(t1, o->center[0], o->center[1], o->center[2]);
    mat4x4_mul(m, m, t1);
    mat4x4_mul(m, m, mat_rot);
    mat4x4 t2;
    mat4x4_translate(t2, -o->center[0], -o->center[1], -o->center[2]);
    mat4x4_mul(m, m, t2);

    glUniformMatrix4fv(glGetUniformLocation(shader_id, "M"), 1, GL_FALSE, (const GLfloat*) m);
    
    // render params
    glUniform1i(glGetUniformLocation(shader_id, "glowing"), o->glowing);
    glUniform3fv(glGetUniformLocation(shader_id, "glow_color"), 1, o->glow_color);
    glUniform1i(glGetUniformLocation(shader_id, "receive_shadows"), o->receive_shadows);

    for (int i = 0; i < o->num_meshes; i++) {
      mesh* mesh = &o->meshes[i];

      // pass material
      glUniform3fv(glGetUniformLocation(shader_id, "material.diffuse"), 1, mesh->mat.diffuse);
      glUniform3fv(glGetUniformLocation(shader_id, "material.specular"), 1, mesh->mat.specular);

      // bind texture
      if (strlen(mesh->mat.texture_path) > 0) {
        glUniform1i(glGetUniformLocation(mesh->texture_id, "texture1"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mesh->texture_id);
        glUniform1i(glGetUniformLocation(shader_id, "hasTexture"), 1);
      } else {
        glUniform1i(glGetUniformLocation(shader_id, "hasTexture"), 0);
      }

      // render the mesh
      glBindVertexArray(mesh->vao);
      glDrawElements(GL_TRIANGLES, mesh->num_indices, GL_UNSIGNED_INT , 0);
    }

    if (debug)
      render_aabb(o);
  }
}

static void render_debug_quad() {
  if (renderer_debug_vao == 0) {
    float quadVertices[] = {
      // positions        // texture Coords
      -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
      1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
      1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    // setup plane VAO
    glGenVertexArrays(1, &renderer_debug_vao);
    glGenBuffers(1, &renderer_debug_vbo);
    glBindVertexArray(renderer_debug_vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer_debug_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  }
  glBindVertexArray(renderer_debug_vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

void renderer_render_objects(object *objects[], int objects_length, light *lights[], int lights_length, camera *camera, void (*ui_render_callback)(void), int debug)
{
  GLint time;
  float ratio;
  int width, height;

  glfwGetFramebufferSize(window, &width, &height);

  /*-------------------------------------------------------------------*/
  /*------------------------------shadows------------------------------*/
  /*-------------------------------------------------------------------*/
  glClearColor(183.0f / 255.0f, 220.0f / 255.0f, 244.0f / 255.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  mat4x4 light_proj, light_view, light_space;
  float near_plane = 1.0f, far_plane = 5.5f;
  mat4x4_ortho(light_proj, -100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
  vec3 up = { 0.0f, 0.0f, 1.0f };
  vec3 dir = { 0.0f, 0.0f, 0.0f };
  mat4x4_look_at(light_view, lights[0]->position, dir, up);
  mat4x4_mul(light_space, light_proj, light_view);

  // render scene from light's point of view
  glUseProgram(renderer_shadow_shader);
  glUniformMatrix4fv(glGetUniformLocation(renderer_shadow_shader, "lightSpaceMatrix"), 1, GL_FALSE, (const GLfloat*) light_space);

  // reset viewport and clear color
  glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
  glBindFramebuffer(GL_FRAMEBUFFER, renderer_depth_fbo);
  glClear(GL_DEPTH_BUFFER_BIT);
  //glCullFace(GL_FRONT);
  render_objects(objects, objects_length, renderer_shadow_shader, 0);
  //glCullFace(GL_BACK);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  /*-----------------------------------------------------------------*/
  /*------------------------------scene------------------------------*/
  /*-----------------------------------------------------------------*/
  glUseProgram(renderer_main_shader);
  glClearColor(183.0f / 255.0f, 220.0f / 255.0f, 244.0f / 255.0f, 1.0f);
  ratio = width / (float)height;
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // camera position
  GLint uniform_camera_pos = glGetUniformLocation(renderer_main_shader, "cameraPos");
  glUniform3fv(uniform_camera_pos, 1, (const GLfloat*) camera->pos);

  // process lights
  glUniform1i(glGetUniformLocation(renderer_main_shader, "lightsNr"), lights_length);
  for (int i = 0; i < lights_length; i++) {
    char uniform_light_pos[256];
    sprintf(uniform_light_pos, "lightsPos[%d]", i);
    char uniform_light_color[256];
    sprintf(uniform_light_color, "lightsColors[%d]", i);
    glUniform3fv(glGetUniformLocation(renderer_main_shader, uniform_light_pos), 1, (const GLfloat*) lights[i]->position);
    glUniform3fv(glGetUniformLocation(renderer_main_shader, uniform_light_color), 1, (const GLfloat*) lights[i]->color);
  }

  // shadow map to shader
  glUniform1i(glGetUniformLocation(renderer_main_shader, "shadowMap"), 0);
  glUniform1f(glGetUniformLocation(renderer_main_shader, "shadowBias"), renderer_shadow_bias);
  glUniform1i(glGetUniformLocation(renderer_main_shader, "shadowPCFEnabled"), renderer_shadow_pcf_enabled);

  // compute mvp matrix
  mat4x4 v, p;
  vec3 camera_dir;
  vec3_add(camera_dir, camera->pos, camera->front);
  mat4x4_look_at(v, camera->pos, camera_dir, camera->up);
  mat4x4_perspective(p, to_radians(45.0f), ratio, 0.1f, 100.0f);

  // pass mvp to shader
  glUniformMatrix4fv(glGetUniformLocation(renderer_main_shader, "V"), 1, GL_FALSE, (const GLfloat*) v);
  glUniformMatrix4fv(glGetUniformLocation(renderer_main_shader, "P"), 1, GL_FALSE, (const GLfloat*) p);

  // pass light-space matrix to shader
  glUniformMatrix4fv(glGetUniformLocation(renderer_main_shader, "lightSpaceMatrix"), 1, GL_FALSE, (const GLfloat*) light_space);

  // pass time to shader
  glUniform1f(glGetUniformLocation(renderer_main_shader, "time"), (float)glfwGetTime());

  // pass depth map
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, renderer_depth_map);

  // process objects
  render_objects(objects, objects_length, renderer_main_shader, debug);

  /*-----------------------------------------------------------------*/
  /*------------------------------debug------------------------------*/
  /*-----------------------------------------------------------------*/
  glUseProgram(renderer_debug_shader);
  glUniform1f(glGetUniformLocation(renderer_debug_shader, "near_plane"), near_plane);
  glUniform1f(glGetUniformLocation(renderer_debug_shader, "far_plane"), far_plane);
  glUniform1i(glGetUniformLocation(renderer_debug_shader, "depthMap"), 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, renderer_depth_map);
  if (renderer_debug_enabled) render_debug_quad();

  // ui callback
  if (ui_render_callback != NULL) {
    ui_render_callback();
  }

  // reset opengl state
  set_opengl_state();

  // swap buffers and poll events
  glfwSwapBuffers(window);

  // poll events
  glfwPollEvents();
}
