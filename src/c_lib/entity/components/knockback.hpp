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

        float weight;

    void get_hit(Vec3 incident, ItemType item)
    {
        const float FORCE = 2.0f;   // TODO -- get this from item type.
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

    KnockbackComponent() :
        Component(COMPONENT_KNOCKBACK, COMPONENT_INTERFACE_KNOCKBACK),
        weight(1.0f)
    {}
};

} // Components

#endif
