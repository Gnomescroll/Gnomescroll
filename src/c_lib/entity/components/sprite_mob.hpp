#pragma once

#include <t_mob/sprites/_interface.hpp>
#include <entity/component/component.hpp>

namespace Components
{

class SpriteMobComponent: public Component
{
    public:
        t_mob::SpriteMob mob;

    void load_settings_from(const Component* component)
    {
        BEGIN_COPY(SpriteMobComponent);
        this->mob.load_settings_from(&_component->mob);
    }

    virtual void on_ready()
    {
        this->mob.associate_entity(this->entity->id, this->entity->type);
        this->mob.init();
    }

    virtual void on_update()
    {
        auto physics = GET_COMPONENT_INTERFACE(Physics, this->entity);
        IF_ASSERT(physics == NULL) return;
        this->mob.position = physics->get_position();
    }

    virtual ~SpriteMobComponent() {}
    SpriteMobComponent() :
        Component(COMPONENT_SpriteMob, COMPONENT_INTERFACE_SpriteMob)
    {}
};

} // Components
