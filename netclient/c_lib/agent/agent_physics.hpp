#pragma once

#include <agent/agent.hpp>

bool agent_collides_terrain(Agent_state* a);
inline bool can_stand_up(float box_r, float box_h, float x, float y, float z);
inline bool on_ground(float box_r, float x, float y, float z);

inline bool collision_check_final_current(float box_r, float box_h, float x, float y, float z);
inline bool collision_check_final_xy(float box_r, float box_h, float x, float y, float z);
inline bool collision_check_final_z(float box_r, float box_h, float x, float y, float z, bool *top);
