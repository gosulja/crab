#include "state.h"
#include <string.h>

void state_init(State* state) {
    camera_init(&state->camera);
    state->objectCount = 0;
}

void state_add_object(State* state, Object* object) {
    if (state->objectCount < MAX_OBJECTS) {
        state->objects[state->objectCount] = *object;
        state->objectCount++;
    }
}

void state_update(State* state, float deltaTime) {
    camera_update_vectors(&state->camera);

    for (int i = 0; i < state->objectCount; i++) {
        object_update(&state->objects[i]);
    }
}

void state_cleanup(State* state) {
    for (int i = 0; i < state->objectCount; i++) {
        object_cleanup(&state->objects[i]);
    }
}