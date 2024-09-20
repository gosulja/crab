#ifndef COMMON_H
#define COMMON_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    vec3 min;
    vec3 max;
} AABB;

#endif // !COMMON_H
