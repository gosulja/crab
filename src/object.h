#ifndef OBJECT_H
#define OBJECT_H

#include "common.h"

typedef struct {
    unsigned int VAO, VBO, EBO;
    int vertexCount;
    int indexCount;
    vec3 position;
    vec3 rotation;
    vec3 scale;
    vec3 color;
    mat4 model;

    GLuint textureID;
    float  textureScale;
} Object;

void object_init(Object* obj, float* vertices, int vertexCount, unsigned int* indices, int indexCount, vec3 color, const char* texture);

void object_create_cube(Object* obj, vec3 color, const char* texturePath);
void object_create_plane(Object* obj, vec3 color, const char* texturePath);
void object_load_from_obj(Object* obj, const char* filePath, vec3 color, const char* texturePath);

void object_update(Object* obj);
void object_draw(Object* obj, GLuint shader);

void object_cleanup(Object* obj);

#endif