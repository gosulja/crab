#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"

typedef struct {
    vec3 position;
    vec3 front;
    vec3 up;
    vec3 right;
    float yaw;
    float pitch;
    float speed;
    float sensitivity;
    float fov;
} Camera;

void camera_init(Camera* camera);
void camera_update_vectors(Camera* camera);
void camera_get_view_matrix(Camera* camera, mat4 view);
void camera_process_keyboard(Camera* camera, int direction, float deltaTime);
void camera_process_mouse_movement(Camera* camera, float xoffset, float yoffset, GLboolean constrainPitch);
void camera_process_mouse_scroll(Camera* camera, float yoffset);

#endif