#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdio.h>

#include "nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"

#include "ui.h"
#include "state.h"
#include "object.h"
#include "camera.h"
#include "shader.h"
#include "input.h"

#define WINDOW_WIDTH    1200
#define WINDOW_HEIGHT   800

State state;
NVGcontext* vg;
GLuint fontNormal;
GLuint fontMono;
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float fps = 0.0f;
float lastTime = 0.0f;
int frameCount = 0;

vec3 lightPos = {4.5f, 3.0f, 4.5f};
vec3 lightColor = {1.0f, 1.0f, 1.0f};

bool toggle_menu(void) {
    printf("Toggling menu?\n");
    if (state.ui.menu.isOpen) {
        state.ui.menu.isOpen = false;
    } else {
        state.ui.menu.isOpen = true;
    }
    printf("%d\n", state.ui.menu.isOpen);
    return true;
}

void initVG() {
    vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
    if (!vg) {
        fprintf(stderr, "Failed to initialize NanoVG.\n");
        exit(1);
    }

    fontNormal = nvgCreateFont(vg, "sans", "../res/fonts/Roboto-Medium.ttf");
    if (fontNormal == -1) {
        fprintf(stderr, "Failed to load font.\n");
        exit(1);
    }

    fontMono = nvgCreateFont(vg, "mono", "../res/fonts/ProggyClean.ttf");
    if (fontMono == -1) {
        fprintf(stderr, "Failed to load font.\n");
        exit(1);
    }

    ui_init(&state.ui, vg);
}

void cleanupVG() {
    if (vg) {
        nvgDeleteGL3(vg);
        vg = NULL;
    }
}

void drawVG() {
    nvgBeginFrame(vg, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f);

    char debugText[256];
    snprintf(debugText, sizeof(debugText), 
             "fps: %.2f - "
             "camera pos: (%.2f, %.2f, %.2f) - "
             "camera yaw: %.2f - "
             "camera pitch: %.2f - "
             "dt: %.4f",
             fps,
             state.camera.position[0], state.camera.position[1], state.camera.position[2],
             state.camera.yaw,
             state.camera.pitch,
             deltaTime);

    nvgFontSize(vg, 16.0f);
    nvgFontFace(vg, "mono");
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);

    nvgFillColor(vg, nvgRGB(255, 255, 255));
    nvgText(vg, 15, 15, debugText, NULL);
    
    nvgBeginPath(vg);
    nvgRoundedRect(vg, 10, 70, 220, 190, 8);
    nvgFillColor(vg, nvgRGB(50, 50, 50));
    nvgFill(vg);

    nvgFillColor(vg, nvgRGB(255, 255, 255));
    nvgFontSize(vg, 18.0f);
    nvgText(vg, 20, 80, "Debug Menu", NULL);

    ui_draw(&state.ui);

    nvgEndFrame(vg);
}

void setup_debug_menu(State* state) {
    ui_add_slider(&state->ui, 20, 120, 200, 20, "Camera Speed", 0.0f, 10.0f, &state->camera.speed);
    ui_add_slider(&state->ui, 20, 170, 200, 20, "Camera Yaw", -180.0f, 180.0f, &state->camera.yaw);
    ui_add_slider(&state->ui, 20, 220, 200, 20, "Camera Pitch", -89.0f, 89.0f, &state->camera.pitch);
}


void test() {
    printf("HELLO WORLD!\n");
}

bool dest(void) {
    glfwSetWindowShouldClose(state.window, 1);
}

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

    state.window = window;

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return -1;
    }

    state_init(&state);

    Shader shaderProgram;
    s_load(&shaderProgram, "../src/shaders/vert_default.glsl", "../src/shaders/frag_default.glsl");

    Shader aabbProgram;
    s_load(&aabbProgram, "../src/shaders/vert_aabb.glsl", "../src/shaders/frag_aabb.glsl");

    Object cube, baseplate, mesh, wedge, place, light, hut, gun;
    object_create_cube(&cube, (vec3){0.35f, 0.35f, 0.35f}, "../res/textures/sky-blue.png");
    object_create_plane(&baseplate, (vec3){0.15f, 0.15f, 0.15f}, "../res/textures/grid.png");
    object_load_from_obj(&mesh, "../res/objs/test.obj", (vec3){0.5f, 0.5f, 0.2f}, "../res/textures/sky-blue.png");
    object_load_from_obj(&wedge, "../res/objs/wedge.obj", (vec3){0.7, 0.2f, 1.0f}, "../res/textures/sky-blue.png");
    object_load_from_obj(&place, "../res/objs/place.obj", (vec3){0.1, 0.1f, 0.1f}, "../res/textures/sky-blue.png");
    object_load_from_obj(&hut, "../res/objs/hut.obj", (vec3){0.1, 0.1f, 0.1f}, "../res/textures/wood.png");
    object_load_from_obj(&gun, "../res/objs/gun.obj", (vec3){0.1, 0.1f, 0.1f}, NULL);
    object_create_cube(&light, lightColor, NULL);

    baseplate.textureScale = 10.0f;

    glm_vec3_copy(lightPos, light.position);
    glm_vec3_copy((vec3){0.2f, 0.2f, 0.2f}, light.scale);
    glm_vec3_copy((vec3){0.0f, 5.0f, 0.0f}, gun.position);
    glm_vec3_copy((vec3){0.2f, 0.2f, 0.2f}, gun.scale);
    glm_vec3_copy((vec3){0.3f, 0.3f, 0.3f}, hut.scale);
    glm_vec3_copy((vec3){5.0f, 0.0f, 5.0f}, hut.position);
    glm_vec3_copy((vec3){0.0f, -1.0f, 0.0f}, baseplate.position);
    glm_vec3_copy((vec3){0.0f, 5.0f, 0.0f}, cube.position);
    glm_vec3_copy((vec3){1.0f, 7.0f, 0.0f}, mesh.position);
    glm_vec3_copy((vec3){-1.0f, 9.0f, 0.0f}, wedge.position);
    glm_vec3_copy((vec3){0.0f, 20.0f, 6.0f}, place.position);
    glm_vec3_copy((vec3){1.0f, 0.1f, 1.0f}, baseplate.scale);

    state_add_object(&state, &cube);
    state_add_object(&state, &baseplate);
    state_add_object(&state, &mesh);
    state_add_object(&state, &wedge);
    state_add_object(&state, &place);
    state_add_object(&state, &light);
    state_add_object(&state, &hut);
    state_add_object(&state, &gun);

    initVG();
    setup_debug_menu(&state);

    state.ui.menu.x = 10;
    state.ui.menu.y = 60;
    state.ui.menu.width = 200;
    state.ui.menu.title = "Main Menu";
    state.ui.menu.isOpen = false;

    ui_add_button(&state.ui, 10, WINDOW_HEIGHT - 50, 100, 40, "Menu", toggle_menu);
    ui_add_button(&state.ui, 120, WINDOW_HEIGHT - 50, 100, 40, "Destroy", dest);

    ui_add_menu_item(&state.ui, "New Game", test);
    ui_add_menu_item(&state.ui, "Options", test);
    ui_add_menu_item(&state.ui, "Quit", test);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        frameCount++;
        if (currentFrame - lastTime >= 1.0) {
            fps = frameCount / (currentFrame - lastTime);
            lastTime = currentFrame;
            frameCount = 0;
        }

        process_input(window, &state, deltaTime);

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        ui_handle_mouse(&state.ui, xpos, ypos, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);

        glClearColor(0.15f, 0.151f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        s_use(&shaderProgram);

        mat4 view, projection;
        camera_get_view_matrix(&state.camera, view);
        glm_perspective(glm_rad(state.camera.fov), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f, projection);

        s_setMat4(&shaderProgram, "view", (float*)view);
        s_setMat4(&shaderProgram, "projection", (float*)projection);
        s_setFloat(&shaderProgram, "time", glfwGetTime());

        s_setVec3(&shaderProgram, "lightPos", lightPos);
        s_setVec3(&shaderProgram, "lightColor", lightColor);
        s_setVec3(&shaderProgram, "viewPos", state.camera.position);

        state_update(&state, deltaTime);

        for (int i = 0; i < state.objectCount; i++) {
            object_draw(&state.objects[i], shaderProgram.id);
        }

        drawVG();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    s_destroy(&shaderProgram);
    object_cleanup(&mesh);
    cleanupVG();

    glfwTerminate();
    return 0;
}
