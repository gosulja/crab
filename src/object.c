#include "object.h"
#include "primitives.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FAST_OBJ_IMPLEMENTATION
#include <fast_obj.h>

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
    stbi_set_flip_vertically_on_load(true);  // Add this line to flip textures vertically
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
        LOG_INFO("Texture loaded successfully: %s (%dx%d, %d channels)", path, width, height, nrChannels);
    } else {
        LOG_ERROR("Failed to load texture at '%s': %s", path, stbi_failure_reason());
        glDeleteTextures(1, &textureID);
        return 0;
    }

    stbi_image_free(data);
    return textureID;
}

void object_init(Object* obj, float* vertices, int vertexCount, unsigned int* indices, int indexCount, vec3 color, const char* texturePath) {
    glGenVertexArrays(1, &obj->VAO);
    glGenBuffers(1, &obj->VBO);

    if (obj->VAO == 0 || obj->VBO == 0) {
        LOG_ERROR("Failed to generate VAO or VBO");
        return;
    }

    glBindVertexArray(obj->VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, obj->VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 8 * sizeof(float), vertices, GL_STATIC_DRAW);

    if (indices != NULL && indexCount > 0) {
        glGenBuffers(1, &obj->EBO);
        if (obj->EBO == 0) {
            LOG_ERROR("Failed to generate EBO");
            return;
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    }

    // position attribute (3 floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal attribute (3 floats)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coordinate attribute (2 floats)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    obj->vertexCount = vertexCount;
    obj->indexCount = indexCount;

    obj->textureScale = 1.0f;

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
    } else {
        obj->textureID = 0;
    }
}

void object_create_cube(Object* obj, vec3 color, const char* texturePath) {
    int vertexCount;
    float* vertices = generate_cube(&vertexCount);
    
    // Convert the 5-component vertices to 8-component vertices (add normals)
    float* newVertices = (float*)malloc(vertexCount * 8 * sizeof(float));
    for (int i = 0; i < vertexCount; i++) {
        // Position
        newVertices[i * 8 + 0] = vertices[i * 5 + 0];
        newVertices[i * 8 + 1] = vertices[i * 5 + 1];
        newVertices[i * 8 + 2] = vertices[i * 5 + 2];
        
        // Normal (simplified, using position as normal)
        newVertices[i * 8 + 3] = vertices[i * 5 + 0];
        newVertices[i * 8 + 4] = vertices[i * 5 + 1];
        newVertices[i * 8 + 5] = vertices[i * 5 + 2];
        
        // Texture coordinates
        newVertices[i * 8 + 6] = vertices[i * 5 + 3];
        newVertices[i * 8 + 7] = vertices[i * 5 + 4];
    }
    
    object_init(obj, newVertices, vertexCount, NULL, 0, color, texturePath);
    free(vertices);
    free(newVertices);
}

void object_create_plane(Object* obj, vec3 color, const char* texturePath) {
    int vertexCount;
    float* vertices = generate_plane(&vertexCount);
    
    // Convert the 5-component vertices to 8-component vertices (add normals)
    float* newVertices = (float*)malloc(vertexCount * 8 * sizeof(float));
    for (int i = 0; i < vertexCount; i++) {
        // Position
        newVertices[i * 8 + 0] = vertices[i * 5 + 0];
        newVertices[i * 8 + 1] = vertices[i * 5 + 1];
        newVertices[i * 8 + 2] = vertices[i * 5 + 2];
        
        // Normal (for plane, always pointing up)
        newVertices[i * 8 + 3] = 0.0f;
        newVertices[i * 8 + 4] = 1.0f;
        newVertices[i * 8 + 5] = 0.0f;
        
        // Texture coordinates
        newVertices[i * 8 + 6] = vertices[i * 5 + 3];
        newVertices[i * 8 + 7] = vertices[i * 5 + 4];
    }
    
    object_init(obj, newVertices, vertexCount, NULL, 0, color, texturePath);
    free(vertices);
    free(newVertices);
}

void object_update(Object* obj) {
    glm_mat4_identity(obj->model);
    glm_translate(obj->model, obj->position);
    glm_rotate(obj->model, obj->rotation[0], (vec3){1.0f, 0.0f, 0.0f});
    glm_rotate(obj->model, obj->rotation[1], (vec3){0.0f, 1.0f, 0.0f});
    glm_rotate(obj->model, obj->rotation[2], (vec3){0.0f, 1.0f, 1.0f});
    glm_scale(obj->model, obj->scale);
}

void object_load_from_obj(Object* obj, const char* objFilePath, vec3 color, const char* texturePath) {
    fastObjMesh* mesh = fast_obj_read(objFilePath);
    if (!mesh) {
        LOG_ERROR("Failed to load OBJ file: %s", objFilePath);
        return;
    }

    vec3 minBounds = {FLT_MAX, FLT_MAX, FLT_MAX};
    vec3 maxBounds = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

    // Count the number of vertices and indices
    unsigned int vertexCount = 0;
    unsigned int indexCount = 0;
    
    for (unsigned int i = 0; i < mesh->face_count; i++) {
        indexCount += 3 * (mesh->face_vertices[i] - 2); // Triangle fan
        vertexCount += mesh->face_vertices[i];
    }

    // Allocate memory for vertices and indices
    float* vertices = (float*)malloc(vertexCount * 8 * sizeof(float));
    unsigned int* indices = (unsigned int*)malloc(indexCount * sizeof(unsigned int));

    if (!vertices || !indices) {
        LOG_ERROR("Failed to allocate memory for mesh data");
        fast_obj_destroy(mesh);
        free(vertices);
        free(indices);
        return;
    }

    // Fill the vertices and indices arrays
    unsigned int vertexOffset = 0;
    unsigned int indexOffset = 0;
    unsigned int faceFirstVertex = 0;

    for (unsigned int i = 0; i < mesh->face_count; i++) {
        for (unsigned int j = 0; j < mesh->face_vertices[i]; j++) {
            fastObjIndex idx = mesh->indices[faceFirstVertex + j];

            // Position
            // vertices[vertexOffset * 8 + 0] = mesh->positions[3 * idx.p + 0];
            // vertices[vertexOffset * 8 + 1] = mesh->positions[3 * idx.p + 1];
            // vertices[vertexOffset * 8 + 2] = mesh->positions[3 * idx.p + 2];

            float x = mesh->positions[3 * idx.p + 0];
            float y = mesh->positions[3 * idx.p + 1];
            float z = mesh->positions[3 * idx.p + 2];

            vertices[vertexOffset * 8 + 0] = x;
            vertices[vertexOffset * 8 + 1] = y;
            vertices[vertexOffset * 8 + 2] = z;

            if (x < minBounds[0]) minBounds[0] = x;
            if (y < minBounds[1]) minBounds[1] = y;
            if (z < minBounds[2]) minBounds[2] = z;
            if (x > maxBounds[0]) maxBounds[0] = x;
            if (y > maxBounds[1]) maxBounds[1] = y;
            if (z > maxBounds[2]) maxBounds[2] = z;

            // Normal (use default if not provided)
            if (idx.n != 0) {
                vertices[vertexOffset * 8 + 3] = mesh->normals[3 * idx.n + 0];
                vertices[vertexOffset * 8 + 4] = mesh->normals[3 * idx.n + 1];
                vertices[vertexOffset * 8 + 5] = mesh->normals[3 * idx.n + 2];
            } else {
                vertices[vertexOffset * 8 + 3] = 0.0f;
                vertices[vertexOffset * 8 + 4] = 1.0f;
                vertices[vertexOffset * 8 + 5] = 0.0f;
            }

            // Texture coordinates (use default if not provided)
            if (idx.t != 0) {
                vertices[vertexOffset * 8 + 6] = mesh->texcoords[2 * idx.t + 0];
                vertices[vertexOffset * 8 + 7] = mesh->texcoords[2 * idx.t + 1];
            } else {
                vertices[vertexOffset * 8 + 6] = 0.0f;
                vertices[vertexOffset * 8 + 7] = 0.0f;
            }

            vertexOffset++;
        }

        // Generate indices for triangle fan
        for (unsigned int j = 1; j < mesh->face_vertices[i] - 1; j++) {
            indices[indexOffset++] = faceFirstVertex;
            indices[indexOffset++] = faceFirstVertex + j;
            indices[indexOffset++] = faceFirstVertex + j + 1;
        }

        faceFirstVertex += mesh->face_vertices[i];
    }

    vec3 center;
    glm_vec3_add(minBounds, maxBounds, center);
    glm_vec3_scale(center, 0.5f, center);

    // Offset the object so it's centered at the origin
    for (unsigned int i = 0; i < vertexCount; i++) {
        vertices[i * 8 + 0] -= center[0];
        vertices[i * 8 + 1] -= center[1];
        vertices[i * 8 + 2] -= center[2];
    }

    // Initialize the object with the loaded data
    object_init(obj, vertices, vertexCount, indices, indexCount, color, texturePath);

    // Free the allocated memory
    free(vertices);
    free(indices);
    fast_obj_destroy(mesh);

    LOG_INFO("Successfully loaded OBJ file: %s (Vertices: %d, Indices: %d)", objFilePath, vertexCount, indexCount);
}

void object_draw(Object* obj, GLuint shader) {
    glUseProgram(shader);

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
    if (colorLoc == -1) {
        LOG_ERROR("Failed to find uniform 'color'");
    } else {
        glUniform3fv(colorLoc, 1, obj->color);
    }

    glBindVertexArray(obj->VAO);
    if (obj->indexCount > 0) {
        glDrawElements(GL_TRIANGLES, obj->indexCount, GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, obj->vertexCount);
    }
}

void object_cleanup(Object* obj) {
    glDeleteVertexArrays(1, &obj->VAO);
    glDeleteBuffers(1, &obj->VBO);
    glDeleteBuffers(1, &obj->EBO);
    glDeleteTextures(1, &obj->textureID);
}