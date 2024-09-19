#ifndef INPUT_H
#define INPUT_H

#include "common.h"
#include "camera.h"

void process_input(GLFWwindow* window, Camera* camera, float deltaTime);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

#endif