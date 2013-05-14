#pragma once

#include <agent/agent.hpp>
#include <agent/constants.hpp>
#include <physics/common.hpp>

bool object_collides_terrain(Vec3 position, float height, float radius);

namespace Agents
{

bool agent_collides_terrain(class Agent* a);
inline bool can_stand_up(float radius, float box_h, float x, float y, float z);
inline bool on_ground(float radius, float x, float y, float z);
inline bool collision_check_current(float radius, float box_h, float x, float y, float z);
inline int clamp_to_ground(float radius, float x, float y, float z);

bool move_with_collision(const BoundingBox& box, Vec3& position, Vec3& velocity);
void apply_control_state(const AgentControlState& cs, Vec3& position,
                         Vec3& velocity, float ground_distance);

class AgentState agent_tick(const struct AgentControlState& _cs,
                            const struct BoundingBox& box,
                            class AgentState as);

}   // Aents
