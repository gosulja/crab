#include "input.h"

extern float lastX;
extern float lastY;
extern bool firstMouse;

void process_input(GLFWwindow* window, Camera* camera, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera_process_keyboard(camera, 0, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera_process_keyboard(camera, 1, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera_process_keyboard(camera, 2, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera_process_keyboard(camera, 3, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    extern Camera camera;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera_process_mouse_movement(&camera, xoffset, yoffset, GL_TRUE);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    extern Camera camera;
    camera_process_mouse_scroll(&camera, yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}