#pragma once

#include <agent/agent.hpp>
#include <entity/component/component.hpp>

namespace Components
{

class AgentTargetingComponent: public TargetingComponent
{
    protected:

        int jump_cooldown_tick;
        int jump_cooldown_max;
        int jump_cooldown_en_route;  // when moving to agent, but not near it
        int jump_cooldown_nearby;    // when near an agent
        int attack_tick;

    Agents::Agent* get_agent()
    {
        if (this->target_id == NULL_AGENT) return NULL;
        return Agents::get_agent((AgentID)this->target_id);
    }

    float get_target_distance(const Vec3& position)
    {
        Agents::Agent* agent = this->get_agent();
        if (agent == NULL) return 0.0f;
        return vec3_distance_squared(agent->get_position(), position);
    }

    Vec3 get_target_position()
    {
        Agents::Agent* agent = this->get_agent();
        if (agent == NULL) return vec3_init(0);
        return agent->get_position();
    }

    public:
        // config
        float speed;
        float proximity_radius;
        int max_z_diff;
        int max_lock_ticks;
        float jump_force;
        int attack_rate;
        int attack_damage;
        int stuck_threshold;
        bool jump_near_player;

        // state
        int ticks_locked;
        int ticks_stuck;

    void set_jump_cooldowns(int en_route, int nearby)
    {   // en_route is while its moving towards player,
        // nearby is when its up close
        this->jump_cooldown_en_route = en_route;
        this->jump_cooldown_nearby = nearby;
    }

    void check_target_alive();

    // explicitly set the target
    void set_target(AgentID agent_id);

    // lock to agent
    void lock_target(Vec3 camera_position);

    void orient_to_target(Vec3 camera_position);

    void move_on_surface();

    void call();

    void load_settings_from(const Component* component)
    {
        BEGIN_COPY(AgentTargetingComponent);
        COPY(speed);
        COPY(proximity_radius);
        COPY(max_z_diff);
        COPY(max_lock_ticks);
        COPY(jump_force);
        COPY(attack_rate);
        COPY(attack_damage);
        COPY(jump_near_player);
    }

    virtual ~AgentTargetingComponent() {}

    AgentTargetingComponent() :
        TargetingComponent(COMPONENT_AgentTargeting),
        jump_cooldown_tick(0), jump_cooldown_max(ONE_SECOND),
        jump_cooldown_en_route(ONE_SECOND), jump_cooldown_nearby(ONE_SECOND),
        attack_tick(0), speed(1.0f), proximity_radius(0.25f), max_z_diff(128),
        max_lock_ticks(0), jump_force(0.38f), attack_rate(ONE_SECOND),
        attack_damage(10), stuck_threshold(6), jump_near_player(false),
        ticks_locked(0), ticks_stuck(0)
    {
        this->target_id = NULL_AGENT;
    }
};

} // Components
