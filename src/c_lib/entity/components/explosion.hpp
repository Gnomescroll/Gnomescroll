/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#if DC_CLIENT
#error do not include this file in client
#endif

#include <entity/component/component.hpp>
#include <t_map/t_map.hpp>

namespace Components
{

class ExplosionComponent: public Component
{
    private:
        int get_block_damage(float distance);

    public:
        float radius;
        int damage;
        float proximity_radius;
        bool harms_owner; // if true, owner of exploding entity will take damage
        int delay;
        int delay_tick;

        float block_destruction_radius;
        int block_damage;
        TerrainModificationAction terrain_modification_action;

    void explode();
    bool proximity_check(); // check if near agent; if so, die

    void damage_blocks();

    void load_settings_from(const Component* component)
    {
        BEGIN_COPY(ExplosionComponent);
        COPY(radius);
        COPY(proximity_radius);
        COPY(block_destruction_radius);
        COPY(block_damage);
        COPY(terrain_modification_action);
        COPY(damage);
        COPY(harms_owner);
        COPY(delay);
    }

    virtual void on_destroy()
    {
        this->explode();
        this->damage_blocks();
    }

    virtual void on_after_tick()
    {
        this->proximity_check();
    }

    virtual ~ExplosionComponent() {}

    ExplosionComponent() :
        Component(COMPONENT_Explosion, COMPONENT_INTERFACE_Explosion),
        radius(1.0f), damage(0), proximity_radius(1.0f), harms_owner(false),
        delay(0), delay_tick(0), block_destruction_radius(1.0f),
        block_damage(32), terrain_modification_action(TMA_NONE)
    {}
};

} // Components
