#pragma once

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/components/health.hpp>

namespace Components
{

class HitPointsHealthComponent: public HealthComponent
{
    private:
        bool died;
        
    public:
        int health;
        int max_health;

        void take_damage(int dmg)
        {
            if (this->health <= 0) return;
            this->health -= dmg;
            this->health = (this->health < 0) ? 0 : this->health; // floor at 0
            if (this->health <= 0) this->died = true;
        }

        bool did_die()
        {
            bool ret = this->died;
            this->died = false;     // reset due to read
            return ret;
        }
    
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
