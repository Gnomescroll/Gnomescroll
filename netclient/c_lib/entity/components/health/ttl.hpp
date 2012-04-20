#pragma once

namespace Components
{

class TTLHealthComponent: public HealthComponent
{
    public:
        int ttl;
        int ttl_max;
    
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

    TTLHealthComponent()
    : HealthComponent(COMPONENT_TTL),
    ttl(0), ttl_max(30*5)
    {}
};

}
