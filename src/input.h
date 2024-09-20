#ifndef INPUT_H
#define INPUT_H

#include "common.h"
#include "state.h"

void process_input(GLFWwindow* window, State* state, float deltaTime);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

#endif