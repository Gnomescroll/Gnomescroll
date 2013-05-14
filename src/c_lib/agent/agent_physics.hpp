#pragma once

#include <agent/agent.hpp>
#include <agent/constants.hpp>
#include <physics/common.hpp>

bool object_collides_terrain(Vec3 position, float height, float radius);
inline bool collision_check_current(float radius, float box_h, float x, float y, float z);
inline bool on_ground(float radius, const struct Vec3& p);
inline int clamp_to_ground(float radius, float x, float y, float z);
inline bool can_stand_up(float radius, float box_h, float x, float y, float z);

void apply_control_state(const ControlState& cs, float speed, float jump_force,
                         Vec3& position, Vec3& velocity, float ground_distance);

bool move_with_collision(const BoundingBox& box, Vec3& position, Vec3& velocity,
                         float& ground_distance);

namespace Agents
{

bool agent_collides_terrain(class Agent* a);

class AgentState agent_tick(const struct ControlState& _cs,
                            const struct BoundingBox& box,
                            class AgentState as);

}   // Aents
