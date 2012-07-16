#pragma once

typedef enum
{
    CONTAINER_TYPE_NONE,
    AGENT_CONTAINER,
    AGENT_TOOLBELT,
    AGENT_SYNTHESIZER,
    AGENT_ENERGY_TANKS,
    
    CONTAINER_TYPE_STORAGE_BLOCK_SMALL,
    CONTAINER_TYPE_CRAFTING_BENCH_UTILITY,
    CONTAINER_TYPE_CRYOFREEZER_SMALL,
    CONTAINER_TYPE_SMELTER_ONE,
} ItemContainerType;

typedef enum
{
    NULL_ITEM = 0xffff
} ItemID;

typedef enum
{
    CONTAINER_ACTION_NONE = 0,      // null
    
    FULL_HAND_TO_WORLD,             // remove
    PARTIAL_HAND_TO_WORLD,

    // inserts
    FULL_HAND_TO_EMPTY_SLOT,        // drop
    FULL_HAND_TO_OCCUPIED_SLOT,     // merge full stack

    PARTIAL_HAND_TO_EMPTY_SLOT,     // insert partial stack
    PARTIAL_HAND_TO_OCCUPIED_SLOT,  // merge partial stack

    // pickups
    PARTIAL_SLOT_TO_EMPTY_HAND,     // create partial stack in hand
    FULL_SLOT_TO_EMPTY_HAND,        // pickup

    // both
    FULL_HAND_SWAP_WITH_SLOT,       // swap hand with slot

    // synthesizer shopping
    PURCHASE_ITEM_FROM_SYNTHESIZER,

    // crafting
    CRAFT_ITEM_FROM_BENCH,

    // pickup
    FULL_WORLD_TO_EMPTY_SLOT,
    FULL_WORLD_TO_OCCUPIED_SLOT,
    PARTIAL_WORLD_TO_OCCUPIED_SLOT,
    
} ContainerActionType;

typedef enum
{
    IG_NONE,    // null value
    IG_ERROR,
    IG_FIST,
    IG_RESOURCE,    //does nothing, resources, stackable
    IG_PLACER,  //consumed to create block
    IG_HITSCAN_WEAPON,
    IG_MELEE_WEAPON,
    IG_MINING_LASER,
    IG_SHOVEL,
    IG_GRENADE_LAUNCHER,
    IG_SYNTHESIZER_COIN,
    IG_DEBUG,       // debug only items
    IG_UNKNOWN,     // used by recipes
    IG_CONSUMABLE,
} ItemGroup;

typedef enum
{
    IL_NOWHERE,
    IL_PARTICLE,
    IL_CONTAINER,
    IL_HAND,
} ItemLocationType;

/* No typedef for ItemType, use string names */
