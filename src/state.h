#ifndef STATE_H
#define STATE_H

#include "camera.h"
#include "object.h"
#include "ui.h"

#define MAX_OBJECTS 1000

typedef struct {
    Camera      camera;
    GLFWwindow* window;
    UI          ui;
    Object      objects[MAX_OBJECTS];
    int         objectCount;
} State;

void state_init(State* state);
void state_add_object(State* state, Object* object);
void state_update(State* state, float deltaTime);
void state_cleanup(State* state);

#endif