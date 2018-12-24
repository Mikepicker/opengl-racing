#include "editor.h"

void editor_init(GLFWwindow* window, object* objects[], unsigned int size) {
  editor_objects = objects;
  editor_size = size;
  editor_window = window;
}

void editor_update() {
  if (glfwGetKey(editor_window, GLFW_KEY_E)) {
    editor_current_index = (editor_current_index + 1) % editor_size;
  }
}

object* editor_current_object() {
  return editor_objects[editor_current_index];
}
