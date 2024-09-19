#include "shader.h"

char* read_file(const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
      fprintf(stderr, "Could not open file \"%s\".\n", path);
      exit(74);
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL) {
      fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
      exit(74);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
      fprintf(stderr, "Could not read file \"%s\".\n", path);
      exit(74);
    }

    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

void s_load(Shader* shader, const char* vert_path, const char* frag_path) {
  char* vert_code = read_file(vert_path);
  char* frag_code = read_file(frag_path);

  GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert_shader, 1, (const GLchar**)&vert_code, NULL);
  glCompileShader(vert_shader);

  // check compile errors
  int success;
  char info_log[512];
  glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vert_shader, 512, NULL, info_log);
    printf("Vertex shader compilation error: %s\n", info_log);
  }

  GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag_shader, 1, (const GLchar**)&frag_code, NULL);
  glCompileShader(frag_shader);

  // check compile errors
  glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(frag_shader, 512, NULL, info_log);
    printf("Fragment shader compilation error: %s\n", info_log);
  }

  shader->id = glCreateProgram();
  glAttachShader(shader->id, vert_shader);
  glAttachShader(shader->id, frag_shader);
  glLinkProgram(shader->id);

  // check linking errors
  glGetProgramiv(shader->id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shader->id, 512, NULL, info_log);
    printf("Shader linking error: %s\n", info_log);
  }

  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);

  free(vert_code);
  free(frag_code);
}

void s_destroy(Shader* shader) {
	glDeleteProgram(shader->id);
}

void s_use(Shader* shader) {
  glUseProgram(shader->id);
}

void s_setMat4(Shader* shader, const char* name, const GLfloat* value) {
    glUniformMatrix4fv(glGetUniformLocation(shader->id, name), 1, GL_FALSE, value);
}

void s_setVec3(Shader* shader, const char* name, const GLfloat* value) {
    glUniform3fv(glGetUniformLocation(shader->id, name), 1, value);
}

void s_setInt(Shader* shader, const char* name, int value) {
    glUniform1i(glGetUniformLocation(shader->id, name), value);
}

void s_setFloat(Shader* shader, const char* name, float value) {
	glUniform1f(glGetUniformLocation(shader->id, name), value);
}
