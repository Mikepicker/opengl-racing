#include "ui.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT

#include "../libs/nuklear.h"
#include "../libs/nuklear_glfw_gl3.h"

struct nk_context *ctx;
struct nk_colorf bg;

void ui_init() {

 ctx = nk_glfw3_init(microdrag.window, NK_GLFW3_INSTALL_CALLBACKS);
 {struct nk_font_atlas *atlas;
   nk_glfw3_font_stash_begin(&atlas);
   nk_glfw3_font_stash_end();}

 bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
 ui_debug = 1;
}

void ui_render() {

  nk_glfw3_new_frame();

  camera* cam = &microdrag.game_camera;

  /* GUI */
  if (nk_begin(ctx, "Microdrag", nk_rect(50, 50, 300, 400),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
        NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
  {
    enum {EASY, HARD};
    static int op = EASY;
    static int property = 20;

    nk_layout_row_static(ctx, 30, 120, 1);
    if (nk_button_label(ctx, "Toggle AABB"))
      ui_debug = ui_debug == 0 ? 1 : 0;

    nk_layout_row_static(ctx, 30, 120, 1);
    if (nk_button_label(ctx, "Editor on"))
      editor_set_enabled(game_editor.enabled == 0 ? 1 : 0);

    nk_layout_row_static(ctx, 30, 120, 1);
    if (nk_button_label(ctx, "Reset camera")) {
      cam->pos[0] = 0.0f; 
      cam->pos[1] = 2.0f; 
      cam->pos[2] = 9.0f; 
      cam->front[0] = 0.0f;
      cam->front[1] = 0.0f;
      cam->front[2] = -1.0f;
    }

    nk_layout_row_static(ctx, 30, 120, 1);
    if (nk_button_label(ctx, "Compile shader")) {
      renderer_recompile_shader();
    }

    nk_layout_row_static(ctx, 30, 120, 1);
    if (nk_button_label(ctx, "Set light here")) {
      vec3_copy(microdrag.lights[0].position, cam->pos);
    }

    nk_layout_row_static(ctx, 30, 120, 1);
    if (nk_button_label(ctx, "Toggle debug")) {
      renderer_debug_enabled = renderer_debug_enabled == 0 ? 1 : 0;
    }

    nk_layout_row_begin(ctx, NK_STATIC, 30, 2);
    {
      nk_layout_row_push(ctx, 50);
      nk_label(ctx, "Shadow Bias:", NK_TEXT_LEFT);
      nk_layout_row_push(ctx, 110);
      nk_slider_float(ctx, 0, &renderer_shadow_bias, 0.5f, 0.1f);
    }

    char shadow_bias[128];
    snprintf(shadow_bias, 128, "%f\n", renderer_shadow_bias);
    nk_label(ctx, shadow_bias, NK_TEXT_LEFT);

    char camera_pos[128];
    snprintf(camera_pos, 128, "camera: %f %f %f\n", cam->pos[0], cam->pos[1], cam->pos[2]);
    nk_label(ctx, camera_pos, NK_TEXT_LEFT);

  }
  nk_end(ctx);

  nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
}

void ui_free() {
  nk_glfw3_shutdown();
}
