#include "components.hpp"

namespace Components
{

static ComponentInterfaceType* component_interface_map = NULL;

ComponentInterfaceType get_interface_for_component(ComponentType component)
{
    GS_ASSERT(component_interface_map != NULL);
    GS_ASSERT(component >= 0);
    GS_ASSERT(component < MAX_COMPONENT_TYPES);
    if (component_interface_map == NULL) return COMPONENT_INTERFACE_NONE;
    if (component < 0) return COMPONENT_INTERFACE_NONE;
    if (component >= MAX_COMPONENT_TYPES) return COMPONENT_INTERFACE_NONE;
    return component_interface_map[component];
}

static void set_interface_for_component(ComponentType component, ComponentInterfaceType interface)
{
    GS_ASSERT(component_interface_map != NULL);
    GS_ASSERT(component >= 0);
    GS_ASSERT(component < MAX_COMPONENT_TYPES);
    if (component_interface_map == NULL) return;
    if (component < 0) return;
    if (component >= MAX_COMPONENT_TYPES) return;
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

    set_interface_for_component(COMPONENT_OWNER, COMPONENT_INTERFACE_OWNER);

    set_interface_for_component(COMPONENT_AGENT_SPAWNER, COMPONENT_INTERFACE_SPAWNER);
    set_interface_for_component(COMPONENT_MONSTER_SPAWNER, COMPONENT_INTERFACE_SPAWNER);

    set_interface_for_component(COMPONENT_SPAWN_CHILD, COMPONENT_INTERFACE_SPAWN_CHILD);

    set_interface_for_component(COMPONENT_MOTION_TARGETING, COMPONENT_INTERFACE_TARGETING);
    set_interface_for_component(COMPONENT_WEAPON_TARGETING, COMPONENT_INTERFACE_TARGETING);

    set_interface_for_component(COMPONENT_HEALER, COMPONENT_INTERFACE_HEALING);

    set_interface_for_component(COMPONENT_DESTINATION_TARGETING, COMPONENT_INTERFACE_TARGETING);
    set_interface_for_component(COMPONENT_AGENT_TARGETING, COMPONENT_INTERFACE_TARGETING);

    set_interface_for_component(COMPONENT_STATE_MACHINE, COMPONENT_INTERFACE_STATE_MACHINE);
    set_interface_for_component(COMPONENT_WAITING, COMPONENT_INTERFACE_WAITING);

    set_interface_for_component(COMPONENT_DIMENSION, COMPONENT_INTERFACE_DIMENSION);

    set_interface_for_component(COMPONENT_RATE_LIMIT, COMPONENT_INTERFACE_RATE_LIMIT);

    #if DC_CLIENT
    set_interface_for_component(COMPONENT_VOXEL_ANIMATION, COMPONENT_INTERFACE_ANIMATION);
    #endif

    #if DC_SERVER
    set_interface_for_component(COMPONENT_EXPLOSION, COMPONENT_INTERFACE_EXPLOSION);
    set_interface_for_component(COMPONENT_ITEM_DROP, COMPONENT_INTERFACE_ITEM_DROP);
    #endif

    init_components();
}

void teardown()
{
    free(component_interface_map);
    teardown_components();
}

} // Components
