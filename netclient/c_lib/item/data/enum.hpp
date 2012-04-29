
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
