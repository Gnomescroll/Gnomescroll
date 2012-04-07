#pragma once

#include <c_lib/components/component.hpp>

class HealthProperties/*: public ComponentProperties*/
{
    public:
        int health;
        bool died;  // check this immediately after applying damage to find out if the object went from alive to dead
        bool dead;

    HealthProperties()
    : health (NULL_HEALTH), died(false), dead(false)
    {}
};

class HealthComponent
{
    public:
    
        HealthProperties health_properties;

        //int get_health()
        //{
            //return this->health_properties.health;
        //}

        int take_damage(int dmg)
        {
            int health = this->health_properties.health;
            this->health_properties.died = false;
            if (health <= 0) return 0;
            health -= dmg;
            health = (health < 0) ? 0 : health;
            if (health <= 0)
            {
                this->health_properties.died = true;
                this->health_properties.dead = true;
            }
            this->health_properties.health = health;
            return health;
        }

        bool is_dead()
        {
            return this->health_properties.dead;
        }

        // did it die the last time it took damage?
        bool did_die()
        {
            return this->health_properties.died;
        }
};
