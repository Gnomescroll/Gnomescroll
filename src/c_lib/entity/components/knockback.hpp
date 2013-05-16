#pragma once

#if DC_SERVER

#include <physics/vec3.hpp>
#include <entity/component/component.hpp>

namespace Components
{

class KnockbackComponent: public Component
{
    private:
    public:
        // config
        float weight;
        int knockback_cooldown;

        // state
        int knockback_tick;

    void get_hit(Vec3 incident, ItemType item)
    {
        IF_ASSERT(this->weight <= 0.0f) return;
        if (this->knockback_tick > 0) return;
        this->knockback_tick = this->knockback_cooldown;
        const float FORCE = 1.0f;   // TODO -- get this from item type.
                                    // WARNING -- physics code doesn't like
                                    // high values of this (4.0 is too high)
        PhysicsComponent* physics = (PhysicsComponent*)
            this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
        IF_ASSERT(physics == NULL) return;  // how are you getting knocked back if you dont have physics
        Vec3 v = physics->get_momentum();
        v = vec3_add(v, vec3_scalar_mult(incident, FORCE / this->weight));
        // TODO -- raytrace the bounding box of the mob to the end point,
        // so that it doesn't pass through terrain.
        // Also, add gravity to the server side physics, so it can fall properly
        physics->set_momentum(v);
    }

    void call()
    {
        this->knockback_tick = GS_MAX(this->knockback_tick - 1, 0);
    }

    KnockbackComponent() :
        Component(COMPONENT_KNOCKBACK, COMPONENT_INTERFACE_KNOCKBACK),
        weight(1.0f), knockback_cooldown(ONE_SECOND), knockback_tick(0)
    {}
};

} // Components

#endif
