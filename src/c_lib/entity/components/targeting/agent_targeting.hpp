#pragma once

#include <agent/agent.hpp>
#include <entity/component/component.hpp>

namespace Components
{

class AgentTargetingComponent: public TargetingComponent
{
    protected:

    Agents::Agent* get_agent()
    {
        if (this->target_id == NULL_AGENT) return NULL;
        return Agents::get_agent((AgentID)this->target_id);
    }

    float get_target_distance(Vec3 position)
    {
        Agents::Agent* agent = this->get_agent();
        if (agent == NULL) return 0.0f;
        return vec3_distance_squared(agent->get_position(), position);
    }

    public:
        // config
        float speed;
        float proximity_radius;
        int max_z_diff;
        int max_lock_ticks;

        // state
        int ticks_locked;

    void check_target_alive();

    // explicitly set the target
    void set_target(AgentID agent_id);

    // lock to agent
    void lock_target(Vec3 camera_position);

    void orient_to_target(Vec3 camera_position);

    void move_on_surface();

    void call();

    virtual ~AgentTargetingComponent() {}

    AgentTargetingComponent() :
        TargetingComponent(COMPONENT_AGENT_TARGETING),
        speed(1.0f), proximity_radius(0.25f), max_z_diff(128),
        max_lock_ticks(0), ticks_locked(0)
    {
        this->target_id = NULL_AGENT;
    }
};

} // Components
