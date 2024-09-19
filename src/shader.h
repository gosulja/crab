#ifndef SHADER_H
#define SHADER_H

#include "common.h"

typedef struct Shader {
  GLuint id;
} Shader;

void s_load(Shader* shader, const char* vertexPath, const char* fragmentPath);
void s_use(Shader* shader);
void s_destroy(Shader* shader);
void s_setMat4(Shader* shader, const char* name, const GLfloat* value);
void s_setVec3(Shader* shader, const char* name, const GLfloat* value);
void s_setInt(Shader* shader, const char* name, int value);
void s_setFloat(Shader* shader, const char* name, float value);

#endif