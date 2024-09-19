#include "object.h"
#include "primitives.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define LOG_ERROR(format, ...) fprintf(stderr, "ERROR: " format "\n", ##__VA_ARGS__)
#define LOG_INFO(format, ...) printf("INFO: " format "\n", ##__VA_ARGS__)

GLuint load_texture(const char* path) {
    GLuint textureID = 0;
    glGenTextures(1, &textureID);

    if (textureID == 0) {
        LOG_ERROR("Failed to generate texture ID for '%s'", path);
        return 0;
    }

    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = GL_RGB;
        GLenum internalFormat = GL_RGB;
        
        if (nrChannels == 1) {
            format = GL_RED;
            internalFormat = GL_RED;
        } else if (nrChannels == 4) {
            format = GL_RGBA;
            internalFormat = GL_RGBA;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        LOG_ERROR("Failed to load texture at '%s': %s", path, stbi_failure_reason());
        glDeleteTextures(1, &textureID);
        return 0;
    }

    stbi_image_free(data);
    return textureID;
}

void object_init(Object* obj, float* vertices, int vertexCount, vec3 color, const char* texturePath) {
    glGenVertexArrays(1, &obj->VAO);

    if (obj->VAO == 0) {
        LOG_ERROR("Failed to generate VAO");
        return;
    }

    glGenBuffers(1, &obj->VBO);
    if (obj->VBO == 0) {
        LOG_ERROR("Failed to generate VBO");
        return;
    }

    glBindVertexArray(obj->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, obj->VBO);

    glBufferData(GL_ARRAY_BUFFER, vertexCount * 5 * sizeof(float), vertices, GL_STATIC_DRAW);

    // position attribute (3 floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture coordinate attribute (2 floats) (e.g [0.0f, 1.0f])
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    obj->vertexCount = vertexCount;

    obj->textureScale = 1.0f || glm_vec3_max(obj->scale);

    glm_vec3_copy((vec3){0.0f, 0.0f, 0.0f}, obj->position);
    glm_vec3_copy((vec3){0.0f, 0.0f, 0.0f}, obj->rotation);
    glm_vec3_copy((vec3){1.0f, 1.0f, 1.0f}, obj->scale);
    glm_vec3_copy(color, obj->color);
    glm_mat4_identity(obj->model);

    if (texturePath != NULL) {
        obj->textureID = load_texture(texturePath);
        if (obj->textureID == 0) {
            LOG_ERROR("Failed to load texture '%s'", texturePath);
        }
    }
}

void object_create_cube(Object* obj, vec3 color, const char* texturePath) {
    int vertexCount;
    float* vertices = generate_cube(&vertexCount);
    object_init(obj, vertices, vertexCount, color, texturePath);
    free(vertices);
}

void object_create_plane(Object* obj, vec3 color, const char* texturePath) {
    int vertexCount;
    float* vertices = generate_plane(&vertexCount);
    object_init(obj, vertices, vertexCount, color, texturePath);
    free(vertices);
}

void object_update(Object* obj) {
    glm_mat4_identity(obj->model);
    glm_translate(obj->model, obj->position);
    glm_rotate(obj->model, obj->rotation[0], (vec3){1.0f, 0.0f, 0.0f});
    glm_rotate(obj->model, obj->rotation[1], (vec3){0.0f, 1.0f, 0.0f});
    glm_rotate(obj->model, obj->rotation[2], (vec3){0.0f, 1.0f, 1.0f});
    glm_scale(obj->model, obj->scale);
}

void object_draw(Object* obj, GLuint shader) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, obj->textureID);

	glUniform1f(glGetUniformLocation(shader, "textureScale"), obj->textureScale);

    GLint texLoc = glGetUniformLocation(shader, "texture1");
    if (texLoc == -1) {
        LOG_ERROR("Failed to find uniform 'texture1'");
    } else {
        glUniform1i(texLoc, 0);
    }

    GLint modelLoc = glGetUniformLocation(shader, "model");
    if (modelLoc == -1) {
        LOG_ERROR("Failed to find uniform 'model'");
    } else {
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float*)obj->model);
    }

    GLint colorLoc = glGetUniformLocation(shader, "color");
    glUniform3fv(colorLoc, 1, obj->color);

    glBindVertexArray(obj->VAO);
    glDrawArrays(GL_TRIANGLES, 0, obj->vertexCount);
}