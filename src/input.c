#include "input.h"

extern float lastX;
extern float lastY;
extern bool firstMouse;

void process_input(GLFWwindow* window, State* state, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera_process_keyboard(&state->camera, 0, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera_process_keyboard(&state->camera, 1, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera_process_keyboard(&state->camera, 2, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera_process_keyboard(&state->camera, 3, deltaTime);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        ui_handle_mouse(&state->ui, xpos, ypos, true);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    extern State state;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    ui_handle_mouse(&state.ui, xpos, ypos, false);

    if (!state.ui.menu.isOpen) {
        camera_process_mouse_movement(&state.camera, xoffset, yoffset, GL_TRUE);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    extern State state;
    camera_process_mouse_scroll(&state.camera, yoffset);
}

void toggle_cursor(GLFWwindow* window) {
    static int cursorMode = GLFW_CURSOR_DISABLED;
    cursorMode = (cursorMode == GLFW_CURSOR_DISABLED) ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
    glfwSetInputMode(window, GLFW_CURSOR, cursorMode);
    firstMouse = true;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        toggle_cursor(window);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}