/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
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
        auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
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

    virtual void load_settings_from(const Component* component)
    {
        BEGIN_COPY(KnockbackComponent);
        COPY(weight);
        COPY(knockback_cooldown);
    }

    KnockbackComponent() :
        Component(COMPONENT_Knockback, COMPONENT_INTERFACE_Knockback),
        weight(1.0f), knockback_cooldown(ONE_SECOND), knockback_tick(0)
    {}
};

} // Components

#endif
