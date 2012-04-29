
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

typedef enum
{
    CONTAINER_ACTION_NONE = 0,      // null
    
    FULL_HAND_TO_WORLD,             // remove

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
} ContainerActionType;
