#pragma once

#include <c_lib/objects/common/component/component.hpp>

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

class HealthComponent: public HealthDelegate
{
    public:
    
        HealthProperties properties;

        //int get_health()
        //{
            //return this->properties.health;
        //}

        int take_damage(int dmg)
        {
            int health = this->properties.health;
            this->properties.died = false;
            if (health <= 0) return 0;
            health -= dmg;
            health = (health < 0) ? 0 : health;
            if (health <= 0)
            {
                this->properties.died = true;
                this->properties.dead = true;
            }
            this->properties.health = health;
            return health;
        }

        bool is_dead()
        {
            return this->properties.dead;
        }

        // did it die the last time it took damage?
        bool did_die()
        {
            return this->properties.died;
        }
};
