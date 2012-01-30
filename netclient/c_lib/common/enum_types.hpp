#pragma once

enum Object_types
{
    OBJ_TYPE_AGENT,
    OBJ_TYPE_SLIME,
    OBJ_TYPE_VELOCIRAPTOR,
    OBJ_TYPE_GRENADE,
    OBJ_TYPE_FLAG,
    OBJ_TYPE_BASE,
    OBJ_TYPE_SPAWNER,
    OBJ_TYPE_TURRET
};

#define COST_SPAWNER 3
#define COST_TURRET 1

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
            printf("Agent_status::can_afford -- unknown obj type %d\n", obj);
            return 0;
    }
    return cost;
}
