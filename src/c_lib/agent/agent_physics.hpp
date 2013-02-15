#pragma once

#include <agent/agent.hpp>

bool object_collides_terrain(Vec3 position, float height, float radius);

namespace Agents
{

const float tr = 1.0f / 10.0f;
const float tr2 = tr*tr;
const float AGENT_GRAVITY = -3.0f * tr2;
const float JETPACK_VELOCITY = -AGENT_GRAVITY + tr2;
const float JETPACK_MAX_VELOCITY = JETPACK_VELOCITY * 5.0f;
const float JETPACK_MAX_HEIGHT = 8.0f;


bool agent_collides_terrain(class Agent* a);
inline bool can_stand_up(float box_r, float box_h, float x, float y, float z);
inline bool on_ground(float box_r, float x, float y, float z);
inline bool collision_check_current(float box_r, float box_h, float x, float y, float z);
inline int clamp_to_ground(float box_r, float x, float y, float z);

class AgentState agent_tick(const struct AgentControlState& _cs,
                             const struct AgentCollisionBox& box,
                             class AgentState as);

}   // Aents
