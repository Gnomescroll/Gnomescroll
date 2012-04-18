#pragma once

#include <c_lib/ray_trace/handlers.hpp>

/* TargetAcquistion component */
// TODO: move to c_lib/components/
class TargetAcquisitionComponent
{
    public:
        float target_acquisition_probability;
        int fire_tick;
        int fire_rate_limit;
        Hitscan::AttackerProperties attacker_properties;

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
        
        Agent_state* acquire_target(
            int id, ObjectType type, int team, float camera_z, Vec3 position,
            float accuracy_bias, float sight_range,
            bool attack_enemies, bool attack_random,
            Vec3* firing_direction
        );
        
        Agent_state* fire_on_target(
            int id, ObjectType type, int team, float camera_z, Vec3 position,
            float accuracy_bias, float sight_range,
            bool attack_enemies, bool attack_random
        );

        Agent_state* fire_on_known_target(
            int id, ObjectType type, float camera_z, Vec3 position, Vec3 direction,
            float accuracy_bias, float sight_range, Agent_state* agent
        );

    TargetAcquisitionComponent()
    : target_acquisition_probability(1.0f), fire_tick(0), fire_rate_limit(1)
    {}
};

