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

#include "../engine/nuklear.h"
#include "../engine/nuklear_glfw_gl3.h"

struct nk_context *ctx;
struct nk_colorf bg;

void ui_init(GLFWwindow* window) {

 ctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);
 {struct nk_font_atlas *atlas;
   nk_glfw3_font_stash_begin(&atlas);
   nk_glfw3_font_stash_end();}

 bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
  ui_debug = 1;
}

void ui_render() {

  nk_glfw3_new_frame();

  /* GUI */
  if (nk_begin(ctx, "Microdrag", nk_rect(50, 50, 300, 300),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
        NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
  {
    enum {EASY, HARD};
    static int op = EASY;
    static int property = 20;
    nk_layout_row_static(ctx, 30, 120, 1);
    if (nk_button_label(ctx, "Toggle AABB"))
      ui_debug = ui_debug == 0 ? 1 : 0;

    nk_layout_row_dynamic(ctx, 30, 2);
    if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
    if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

    nk_layout_row_dynamic(ctx, 25, 1);
    nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

    nk_layout_row_dynamic(ctx, 20, 1);
    nk_label(ctx, "background:", NK_TEXT_LEFT);
    nk_layout_row_dynamic(ctx, 25, 1);
    if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx),400))) {
      nk_layout_row_dynamic(ctx, 120, 1);
      bg = nk_color_picker(ctx, bg, NK_RGBA);
      nk_layout_row_dynamic(ctx, 25, 1);
      bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f,0.005f);
      bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f,0.005f);
      bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f,0.005f);
      bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f,0.005f);
      nk_combo_end(ctx);
    }

    char camera_pos[128];
    snprintf(camera_pos, 128, "camera: %f %f %f\n", ui_camera.pos[0], ui_camera.pos[1], ui_camera.pos[2]);
    nk_label(ctx, camera_pos, NK_TEXT_LEFT);

  }
  nk_end(ctx);

  nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
}

void ui_free() {
  nk_glfw3_shutdown();
}

void ui_set_camera(camera cam) { ui_camera = cam; }
