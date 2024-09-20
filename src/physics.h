#ifndef PHYSICS_H
#define PHYSICS_H

#include "common.h"

void update_aabb(vec3 position, vec3 scale, AABB* aabb);
bool check_collision_aabb(const AABB* a, const AABB* b);
void apply_gravity(vec3 velocity, float dt);
void resolve_collision(vec3 pos1, vec3 vel1, vec3 pos2, vec3 vel2, vec3 scale1, vec3 scale2);

#endif