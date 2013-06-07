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

    void get_hit(const Vec3& incident, ItemType item, float force_multiplier=1.0f)
    {
        IF_ASSERT(this->weight <= 0.0f) return;
        if (this->knockback_tick > 0) return;
        this->knockback_tick = this->knockback_cooldown;
        const float FORCE_BASE = 1.0f;   // TODO -- get this from item type.
                                         // WARNING -- physics code doesn't like
                                         // high values of this (4.0 is too high)
        const float FORCE_MAX = 2.0f;    // cap the force so as not to disturb the physics
        PhysicsComponent* physics = (PhysicsComponent*)
            this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
        IF_ASSERT(physics == NULL) return;  // how are you getting knocked back if you dont have physics
        Vec3 v = physics->get_momentum();
        float force = (FORCE_BASE * force_multiplier) / this->weight;
        force = GS_MIN(force, FORCE_MAX);
        v = vec3_add(v, vec3_scalar_mult(incident, force));
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
