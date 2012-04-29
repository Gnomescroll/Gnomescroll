#pragma once

typedef enum
{
    CONTAINER_TYPE_NONE,
    AGENT_CONTAINER,
    AGENT_TOOLBELT,
    AGENT_NANITE,
//    AGENT_BAG,
//    CRAFTING_BENCH
} ItemContainerType;

typedef enum
{
    NULL_ITEM = -1,
    MAX_ITEM = 0xffff   // force enum to be large enough
} ItemID;

/*
typedef enum
{
    ERROR_TYPE,
    GENERIC_TYPE, //does nothing, resources, stackable
    GUN,
    MINING_LASER
} ItemTypes;
*/

typedef enum
{
    IG_ERROR,
    IG_RESOURCE, 	//does nothing, resources, stackable
    IG_PLACER, 	//consumed to create block
    IG_HITSCAN_WEAPON,
    IG_MELEE_WEAPON,
    IG_MINING_LASER,
} ItemGroups;