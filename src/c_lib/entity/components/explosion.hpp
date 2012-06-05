#pragma once

#if DC_SERVER

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
        bool harms_owner; // if true, owner of exploding object will take damage

        float block_destruction_radius;
        int block_damage;
        t_map::TerrainModificationAction terrain_modification_action;

        void explode();
        bool proximity_check(); // check if near agent; if so, die

        void damage_blocks();

    ExplosionComponent()
    : Component(COMPONENT_EXPLOSION, COMPONENT_INTERFACE_EXPLOSION),
    radius(1.0f), damage(0), proximity_radius(1.0f), harms_owner(false),
    block_destruction_radius(1.0f), block_damage(32), terrain_modification_action(t_map::TMA_NONE)
    {}
};

} // Components

#endif
