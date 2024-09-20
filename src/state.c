#include "state.h"
#include "physics.h"
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
        Object* obj = &state->objects[i];
        
        // Apply gravity
        apply_gravity(obj->velocity, deltaTime);

        // Update position based on velocity
        vec3 displacement;
        glm_vec3_scale(obj->velocity, deltaTime, displacement);
        glm_vec3_add(obj->position, displacement, obj->position);

        // Update object's model matrix
        object_update(obj);

        // Update AABB
        update_aabb(obj->position, obj->scale, &obj->aabb);

        // Check for collisions with other objects
        for (int j = i + 1; j < state->objectCount; j++) {
            Object* other = &state->objects[j];
            if (check_collision_aabb(&obj->aabb, &other->aabb)) {
                resolve_collision(obj->position, obj->velocity, other->position, other->velocity, obj->scale, other->scale);
            }
        }

        // Ground collision (assuming ground is at y=0)
        if (obj->position[1] < obj->scale[1]) {
            obj->position[1] = obj->scale[1];
            obj->velocity[1] = 0;
        }
    }
}

void state_cleanup(State* state) {
    for (int i = 0; i < state->objectCount; i++) {
        object_cleanup(&state->objects[i]);
    }
}