#pragma once

enum Object_types
{
    OBJ_TYPE_AGENT = 0,

    OBJ_TYPE_SLIME,
    OBJ_TYPE_VELOCIRAPTOR,

    OBJ_TYPE_GRENADE,

    OBJ_TYPE_FLAG,
    OBJ_TYPE_BASE,

    OBJ_TYPE_SPAWNER,
    OBJ_TYPE_TURRET,
    
    OBJ_TYPE_GRENADE_REFILL,
    OBJ_TYPE_LASER_REFILL,

    OBJ_TYPE_DIRT,
    OBJ_TYPE_STONE,

    OBJ_TYPE_INVENTORY,

    OBJ_TYPE_NONE = 255,
};

typedef enum
{
    DEATH_NORMAL = 0,
    DEATH_HEADSHOT,
    DEATH_GRENADE,
    DEATH_FALL,
    DEATH_BELOW_MAP,
    DEATH_TURRET,
    DEATH_SLIME
} AgentDeathMethod;


#define COST_SPAWNER 5
#define COST_TURRET 2

unsigned int get_object_cost(Object_types obj)
{
    unsigned int cost;
    switch (obj) {
        case OBJ_TYPE_SPAWNER:
            cost = COST_SPAWNER;
            break;
        case OBJ_TYPE_TURRET:
            cost = COST_TURRET;
            break;
        default:
            printf("get_object_cost -- unknown obj type %d\n", obj);
            return 0;
    }
    return cost;
}
