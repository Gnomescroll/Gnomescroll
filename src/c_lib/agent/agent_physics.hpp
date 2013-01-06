#pragma once

#include <agent/agent.hpp>

bool object_collides_terrain(Vec3 position, float height, float radius);

namespace Agents
{

bool agent_collides_terrain(class Agent* a);
inline bool can_stand_up(float box_r, float box_h, float x, float y, float z);
inline bool on_ground(float box_r, float x, float y, float z);

inline bool collision_check_final_current(float box_r, float box_h, float x, float y, float z);
inline bool collision_check_final_xy(float box_r, float box_h, float x, float y, float z);
inline bool collision_check_final_z(float box_r, float box_h, float x, float y, float z, bool *top);

class AgentState agent_tick(const struct AgentControlState& _cs, const struct AgentCollisionBox& box, class AgentState as);

}   // Aents
