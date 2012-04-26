#include "slot.hpp"

#if DC_SERVER
void InventorySlot::load(int id, ObjectType type, int stack_size)
{
    this->item_id = id;
    this->item_type = type;
    this->stack.count = stack_size;
    //this->stack.max = get_max_stack_size(type);
    this->stack.max = 1;
}

#endif

#if DC_CLIENT
void InventorySlot::load(int id, ObjectType type, int stack_size)
{
    #if DC_CLIENT
    this->sprite_index = 1;
    #endif
    this->item_id = id;
    this->item_type = type;
    this->stack.count = stack_size;
    //this->stack.max = get_max_stack_size(type);
    this->stack.max = 1;
    //printf("Loaded inventory item %d,%d\n", this->item_id, this->item_type);
}
#endif
