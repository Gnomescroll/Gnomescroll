#pragma once

#if DC_SERVER
#error do not include this file in server
#endif

#include <common/common.hpp>
#include <entity/constants.hpp>
#include <entity/component/component.hpp>

namespace Components
{

class AnimationComponent: public Component
{
    public:
        int count;
        int count_max;
        float size;
        float force;
        Color color;

    void explode(const Vec3& position);
    void explode_random(const Vec3& position);

    void load_settings_from(const Component* component)
    {
        BEGIN_COPY(AnimationComponent);
        COPY(count);
        COPY(count_max);
        COPY(color);
        COPY(size);
        COPY(force);
    }

    virtual void on_destroy()
    {
        auto physics = GET_COMPONENT_INTERFACE(Physics, this->entity);
        IF_ASSERT(physics == NULL) return;
        float h = 0;
        auto dims = GET_COMPONENT_INTERFACE(Dimension, entity);
        if (dims != NULL)
            h = dims->get_height();
        Vec3 p = physics->get_position();
        p.z += h * 0.5f;
        this->explode_random(p);
    }

    virtual ~AnimationComponent() {}

    AnimationComponent() :
        Component(COMPONENT_Animation, COMPONENT_INTERFACE_Animation),
        count(10), count_max(10), size(0.5f), force(20.0f)
    {   // dont set color
    }
};

} // Entities
