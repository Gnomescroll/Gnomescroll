#pragma once

typedef enum
{
    OBJECT_NONE,
    OBJECT_HEALTH_REFILL,
    OBJECT_SOFT_ROCK_BLOCK_DROP,
} ObjectType;

typedef enum
{
    COMPONENT_NONE,

    COMPONENT_POSITION, // todo -- tag with _PHYSICS
    COMPONENT_POSITION_MOMENTUM,
    COMPONENT_VERLET,
    COMPONENT_STACKABLE,
    COMPONENT_BILLBOARD_SPRITE,
    COMPONENT_PICKUP,
    COMPONENT_TTL,
    COMPONENT_HIT_POINTS,
    
} ComponentType;

typedef enum
{
    COMPONENT_INTERFACE_NONE,
    COMPONENT_INTERFACE_PHYSICS,    // physics state setters/getters
    COMPONENT_INTERFACE_STACKABLE,  // stackable setters/getters
    COMPONENT_INTERFACE_DRAW,       // draw()
    COMPONENT_INTERFACE_UPDATE,     // update()
    COMPONENT_INTERFACE_TICK,       // tick()
    COMPONENT_INTERFACE_PICKUP,
    COMPONENT_INTERFACE_HEALTH,     // is_dead()
    
} ComponentInterfaceType;

//const Vec3 NULL_MOMENTUM = { 0,0,0 };
