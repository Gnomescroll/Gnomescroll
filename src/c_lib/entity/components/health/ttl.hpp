#pragma once

namespace Components
{

class TTLComponent: public HealthComponent
{
    public:
        int ttl;
        int ttl_max;

        void take_damage(int dmg)
        {
            this->die(); // die on any strike
        }

        bool did_die()
        {
            return true;   // assume died under whatever circumstance. should only be used temporarily here
        }

        bool is_dead()
        {
            if (this->ttl >= ttl_max) return true;
            return false;
        }

        void die()
        {
            this->ttl = this->ttl_max;
        }

        void tick()
        {
            if (this->ttl >= 0) // negative ttl means dont tick
                this->ttl++;
        }

    TTLComponent() :
        HealthComponent(COMPONENT_TTL),
        ttl(0), ttl_max(30*5)
    {}
};

}
