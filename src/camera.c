#include "camera.h"

void camera_init(Camera* camera) {
    glm_vec3_copy((vec3){0.0f, 0.0f, 3.0f}, camera->position);
    glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, camera->front);
    glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, camera->up);
    camera->yaw = -90.0f;
    camera->pitch = 0.0f;
    camera->speed = 2.5f;
    camera->sensitivity = 0.1f;
    camera->fov = 75.0f;
    camera_update_vectors(camera);
}

void camera_update_vectors(Camera* camera) {
    vec3 front;
    front[0] = cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
    front[1] = sin(glm_rad(camera->pitch));
    front[2] = sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
    glm_vec3_normalize_to(front, camera->front);
    glm_vec3_cross(camera->front, (vec3){0.0f, 1.0f, 0.0f}, camera->right);
    glm_vec3_normalize(camera->right);
    glm_vec3_cross(camera->right, camera->front, camera->up);
    glm_vec3_normalize(camera->up);
}

void camera_get_view_matrix(Camera* camera, mat4 view) {
    vec3 center;
    glm_vec3_add(camera->position, camera->front, center);
    glm_lookat(camera->position, center, camera->up, view);
}

void camera_process_keyboard(Camera* camera, int direction, float deltaTime) {
    float velocity = camera->speed * deltaTime;
    if (direction == 0) // FORWARD
        glm_vec3_muladds(camera->front, velocity, camera->position);
    if (direction == 1) // BACKWARD
        glm_vec3_muladds(camera->front, -velocity, camera->position);
    if (direction == 2) // LEFT
        glm_vec3_muladds(camera->right, -velocity, camera->position);
    if (direction == 3) // RIGHT
        glm_vec3_muladds(camera->right, velocity, camera->position);
}

void camera_process_mouse_movement(Camera* camera, float xoffset, float yoffset, GLboolean constrainPitch) {
    xoffset *= camera->sensitivity;
    yoffset *= camera->sensitivity;

    camera->yaw   += xoffset;
    camera->pitch += yoffset;

    if (constrainPitch) {
        if (camera->pitch > 89.0f)
            camera->pitch = 89.0f;
        if (camera->pitch < -89.0f)
            camera->pitch = -89.0f;
    }

    camera_update_vectors(camera);
}

void camera_process_mouse_scroll(Camera* camera, float yoffset) {
    camera->fov -= yoffset;
    if (camera->fov < 1.0f)
        camera->fov = 1.0f;
    if (camera->fov > 120.0f)
        camera->fov = 70.0f;
}

