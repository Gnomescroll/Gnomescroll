#include "components.hpp"

namespace Components
{

static ComponentInterfaceType* component_interface_map = NULL;

ComponentInterfaceType get_interface_for_component(ComponentType component)
{
    GS_ASSERT(component_interface_map != NULL);
    GS_ASSERT(component >= 0);
    GS_ASSERT(component < MAX_COMPONENT_TYPES);
    return component_interface_map[component];
}

static void set_interface_for_component(ComponentType component, ComponentInterfaceType interface)
{
    component_interface_map[component] = interface;
}

void init()
{
    component_interface_map = (ComponentInterfaceType*)calloc(MAX_COMPONENT_TYPES, sizeof(ComponentInterfaceType));

    set_interface_for_component(COMPONENT_NONE, COMPONENT_INTERFACE_NONE);

    set_interface_for_component(COMPONENT_POSITION, COMPONENT_INTERFACE_PHYSICS);
    set_interface_for_component(COMPONENT_POSITION_MOMENTUM, COMPONENT_INTERFACE_PHYSICS);
    set_interface_for_component(COMPONENT_POSITION_CHANGED, COMPONENT_INTERFACE_PHYSICS);
    set_interface_for_component(COMPONENT_POSITION_MOMENTUM_CHANGED, COMPONENT_INTERFACE_PHYSICS);
    set_interface_for_component(COMPONENT_VERLET, COMPONENT_INTERFACE_PHYSICS);

    set_interface_for_component(COMPONENT_STACKABLE, COMPONENT_INTERFACE_STACKABLE);

    set_interface_for_component(COMPONENT_PICKUP, COMPONENT_INTERFACE_PICKUP);
    
    set_interface_for_component(COMPONENT_TTL, COMPONENT_INTERFACE_HEALTH);
    set_interface_for_component(COMPONENT_HIT_POINTS, COMPONENT_INTERFACE_HEALTH);

    set_interface_for_component(COMPONENT_VOXEL_MODEL, COMPONENT_INTERFACE_VOXEL_MODEL);

    set_interface_for_component(COMPONENT_TEAM, COMPONENT_INTERFACE_TEAM);
    set_interface_for_component(COMPONENT_INDEXED_TEAM, COMPONENT_INTERFACE_TEAM);

    set_interface_for_component(COMPONENT_OWNER, COMPONENT_INTERFACE_OWNER);

    set_interface_for_component(COMPONENT_AGENT_SPAWNER, COMPONENT_INTERFACE_SPAWNER);
    set_interface_for_component(COMPONENT_MONSTER_SPAWNER, COMPONENT_INTERFACE_SPAWNER);

    set_interface_for_component(COMPONENT_SPAWN_CHILD, COMPONENT_INTERFACE_SPAWN_CHILD);
    
    set_interface_for_component(COMPONENT_MOTION_TARGETING, COMPONENT_INTERFACE_TARGETING);
    set_interface_for_component(COMPONENT_WEAPON_TARGETING, COMPONENT_INTERFACE_TARGETING);

    set_interface_for_component(COMPONENT_DIMENSION, COMPONENT_INTERFACE_DIMENSION);

    set_interface_for_component(COMPONENT_EXPLOSION, COMPONENT_INTERFACE_EXPLOSION);

    set_interface_for_component(COMPONENT_RATE_LIMIT, COMPONENT_INTERFACE_RATE_LIMIT);

    #if DC_CLIENT
    set_interface_for_component(COMPONENT_BILLBOARD_SPRITE, COMPONENT_INTERFACE_DRAW);
    set_interface_for_component(COMPONENT_COLORED_VOXEL, COMPONENT_INTERFACE_DRAW);
    set_interface_for_component(COMPONENT_TEXTURED_VOXEL, COMPONENT_INTERFACE_DRAW);

    set_interface_for_component(COMPONENT_VOXEL_ANIMATION, COMPONENT_INTERFACE_ANIMATION);
    set_interface_for_component(COMPONENT_SPRITE_ANIMATION, COMPONENT_INTERFACE_ANIMATION);
    #endif

    init_components();
}

void teardown()
{
    if (component_interface_map != NULL) free(component_interface_map);

    teardown_components();
}

} // Components
