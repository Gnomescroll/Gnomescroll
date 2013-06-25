#pragma once

#include <entity/component/component.hpp>

namespace Components
{

class WeaponTargetingComponent: public TargetingComponent
{
    public:
        int fire_tick;
        int fire_rate_limit;
        Hitscan::AttackerProperties attacker_properties;

        bool uses_bias;
        float accuracy_bias;
        bool attack_at_random;

        bool locked_on_target;

        int fire_delay;
        int fire_delay_max;

        struct Vec3 firing_direction;
        bool firing_direction_set;

    virtual void load_settings_from(const Component* component)
    {
        BEGIN_COPY(WeaponTargetingComponent);
        COPY(fire_rate_limit);
        COPY(attacker_properties);
        COPY(uses_bias);
        COPY(accuracy_bias);
        COPY(attack_at_random);
        COPY(fire_delay_max);
    }

    void tick()
    {
        this->fire_tick++;
    }

    bool can_lock_part()
    {
        return (this->fire_tick % this->fire_rate_limit == 0);
    }

    bool can_fire()
    {
        return (this->fire_tick %
                (this->fire_rate_limit + this->fire_delay) == 0);
    }

    void set_random_fire_tick()
    {
        this->fire_tick = randrange(0, fire_rate_limit);
    }

    void add_random_fire_delay()
    {
        this->fire_delay = randrange(0, fire_delay_max);
    }

    bool target_is_visible(Vec3 firing_position);
    bool fire_on_target(Vec3 camera_position);
    void orient_to_random_target_part(Vec3 camera_position);
    void lock_target(Vec3 camera_position);
    void lock_target_part(Vec3 camera_position);

    virtual void on_ready()
    {
        this->attacker_properties.id = this->entity->id;
    }

    virtual ~WeaponTargetingComponent() {}

    WeaponTargetingComponent() :
        TargetingComponent(COMPONENT_WeaponTargeting),
        fire_tick(0), fire_rate_limit(1), uses_bias(false),
        accuracy_bias(0.0f), attack_at_random(true), locked_on_target(false),
        fire_delay(0), fire_delay_max(0),
        firing_direction(vec3_init(0)), firing_direction_set(false)
    {
        this->set_random_fire_tick();
    }
};

} // Components
