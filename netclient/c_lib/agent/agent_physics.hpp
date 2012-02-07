#pragma once

#include <c_lib/agent/agent.hpp>

bool agent_collides_terrain(Agent_state* a);
inline bool can_stand_up(float box_r, float x, float y, float z, float current_h, float new_h);
inline bool can_stand_up_inner(int x_min, int x_max, int y_min, int y_max, float z);
inline void collision_check1(float box_r, float box_h, float x, float y, float z, int collision[6]);
inline bool collision_check2(float box_r, float box_h, float x, float y, float z);
inline bool collision_check4(float box_r, float box_h, float x, float y, float z, int *cx, int *cy, int *cz);
inline bool collision_check_short(float box_r, float box_h, float x, float y, float z);
inline bool collision_check5(float box_r, float box_h, float x, float y, float z);
inline bool collision_check5_z(float box_r, float box_h, float x, float y, float z);
inline bool collision_check5_stand_up(float box_r, float box_h, float x, float y, float z);
inline bool collision_check6(float box_r, float box_h, float x, float y, float z);
inline bool collision_check6_z(float box_r, float box_h, float x, float y, float z);
inline bool collision_check6_xy(float box_r, float box_h, float x, float y, float z);
inline bool on_ground(float box_r, float x, float y, float z);
inline bool on_solid_ground(float box_r, float x, float y, float z);
inline int _collision_check(int x, int y, int z);
