#pragma once

#include <c_lib/entity/component/component.hpp>

namespace Components
{

class TargetingComponent: public Component
{
    public:
        float target_acquisition_probability;
        int fire_tick;
        int fire_rate_limit;
        Hitscan::AttackerProperties attacker_properties;

        float accuracy_bias;
        float sight_range;
        bool attack_enemies;
        bool attack_random;

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
        
        Agent_state* acquire_target(int id, ObjectType type, int team, float camera_z, Vec3 position, Vec3* firing_direction);
        Agent_state* fire_on_target(int id, ObjectType type, int team, float camera_z, Vec3 position);
        Agent_state* fire_on_known_target(int id, ObjectType type, float camera_z, Vec3 position, Vec3 direction, Agent_state* agent);

    TargetingComponent()
    : target_acquisition_probability(1.0f), fire_tick(0), fire_rate_limit(1),
    accuracy_bias(0.0f), sight_range(10.0f), attack_enemies(true), attack_random(true)
    {}
};

} // Components
