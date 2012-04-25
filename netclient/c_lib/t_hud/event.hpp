#pragma once

namespace t_hud
{
    
typedef enum
{
    INVENTORY_INPUT_EVENT_NONE,
    INVENTORY_INPUT_EVENT_SWAP_WITHIN,
    INVENTORY_INPUT_EVENT_SWAP_BETWEEN,
    INVENTORY_INPUT_EVENT_REMOVE,
} InventoryInputEventType;

typedef struct
{
    InventoryInputEventType type;
    int inventory;
    int slot;
    int inventory_b;
    int slot_b;
} InventoryInputEvent;

extern InventoryInputEvent inventory_input_event;

} // t_hud
