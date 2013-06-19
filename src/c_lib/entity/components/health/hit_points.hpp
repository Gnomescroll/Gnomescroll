#pragma once

#include <entity/constants.hpp>
#include <entity/components/health.hpp>

namespace Components
{

class HitPointsComponent: public HealthComponent
{
    private:
        bool died;

    public:
        int health;
        int health_max;

    void take_damage(int dmg);

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

    HitPointsComponent() :
        HealthComponent(COMPONENT_HitPoints),
        died(false), health(100), health_max(100)
    {}
};

}   // Components
