#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdio.h>

#include "object.h"
#include "camera.h"
#include "shader.h"
#include "input.h"

#define WINDOW_WIDTH    1200
#define WINDOW_HEIGHT   800

Camera camera;
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "crab", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    camera_init(&camera);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return -1;
    }

    Shader shaderProgram;
    s_load(&shaderProgram, "../src/shaders/vert_default.glsl", "../src/shaders/frag_default.glsl");

    Object cube, baseplate;

    object_create_cube(&cube, (vec3){0.35f, 0.35f, 0.35f}, "../src/textures/stone.jpg");
    object_create_plane(&baseplate, (vec3){0.15f, 0.15f, 0.15f}, "../src/textures/grid.png");

    baseplate.textureScale = 10.0f;

    glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, cube.position);
    glm_vec3_copy((vec3){0.0f, -1.0f, 0.0f}, baseplate.position);
    glm_vec3_copy((vec3){1.0f, 0.1f, 1.0f}, baseplate.scale);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        process_input(window, &camera, deltaTime);

        glClearColor(0.15f, 0.151f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        s_use(&shaderProgram);

        mat4 view, projection;
        camera_get_view_matrix(&camera, view);
        glm_perspective(glm_rad(camera.fov), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f, projection);

        s_setMat4(&shaderProgram, "view", (float*)view);
        s_setMat4(&shaderProgram, "projection", (float*)projection);
        s_setFloat(&shaderProgram, "time", glfwGetTime());

        object_update(&cube);
        object_draw(&cube, shaderProgram.id);

        object_update(&baseplate);
        object_draw(&baseplate, shaderProgram.id);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    s_destroy(&shaderProgram);

    glfwTerminate();
    return 0;
}