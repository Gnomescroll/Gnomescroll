#pragma once

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/components/health.hpp>

namespace Components
{

class HitPointsHealthComponent: public HealthComponent
{
    public:
        int health;
        int max_health;
    
        bool is_dead()
        {
            if (this->health <= 0) return true;
            return false;
        }

        void die()
        {
            this->health = 0;
        }

    HitPointsHealthComponent()
    : HealthComponent(COMPONENT_HIT_POINTS),
    health(100), max_health(100)
    {}
};

}
