#include "physics.h"

#define GRAVITY -9.81f

void update_aabb(vec3 position, vec3 scale, AABB* aabb) {
    glm_vec3_add(position, scale, aabb->max);
    glm_vec3_sub(position, scale, aabb->min);
}

bool check_collision_aabb(const AABB* a, const AABB* b) {
    return (a->min[0] <= b->max[0] && a->max[0] >= b->min[0]) &&
           (a->min[1] <= b->max[1] && a->max[1] >= b->min[1]) &&
           (a->min[2] <= b->max[2] && a->max[2] >= b->min[2]);
}

void apply_gravity(vec3 velocity, float deltaTime) {
    velocity[1] += GRAVITY * deltaTime;
}

void resolve_collision(vec3 pos1, vec3 vel1, vec3 pos2, vec3 vel2, vec3 scale1, vec3 scale2) {
    vec3 midpoint, direction;
    glm_vec3_add(pos1, pos2, midpoint);
    glm_vec3_scale(midpoint, 0.5f, midpoint);

    glm_vec3_sub(pos1, midpoint, direction);
    glm_vec3_normalize(direction);
    glm_vec3_scale(direction, scale1[0] + scale2[0], direction);
    glm_vec3_add(midpoint, direction, pos1);

    glm_vec3_negate(direction);
    glm_vec3_add(midpoint, direction, pos2);

    // Reset velocities (simple bounce effect)
    glm_vec3_negate(vel1);
    glm_vec3_negate(vel2);
    glm_vec3_scale(vel1, 0.5f, vel1); // Dampen the bounce
    glm_vec3_scale(vel2, 0.5f, vel2);
}