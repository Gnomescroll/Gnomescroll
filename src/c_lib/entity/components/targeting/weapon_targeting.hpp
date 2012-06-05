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
        bool attacks_enemies;
        bool attack_at_random;

        bool locked_on_target;

        bool can_fire()
        {
            this->fire_tick++;
            if (this->fire_tick % this->fire_rate_limit == 0)
                return true;
            return false;
        }
        
        void set_random_fire_tick()
        {
            this->fire_tick = randrange(0, fire_rate_limit);
        }

        bool target_is_visible(Vec3 firing_position);
        
        bool fire_on_target(Vec3 camera_position, int team);
        bool fire_on_target(Vec3 camera_position);

        void orient_to_random_target_part(Vec3 camera_position);
        
        void broadcast_target_choice();

        void lock_target(Vec3 camera_position);
        void lock_target(Vec3 camera_position, int team);

    WeaponTargetingComponent()
    : TargetingComponent(COMPONENT_WEAPON_TARGETING),
    fire_tick(0), fire_rate_limit(1), uses_bias(false), accuracy_bias(0.0f),
    attacks_enemies(true), attack_at_random(true), locked_on_target(false)
    {
        this->set_random_fire_tick();
    }
};

} // Components
