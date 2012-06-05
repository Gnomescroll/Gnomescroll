#include "stackable.hpp"

namespace Components
{

bool StackableComponent::merge_stack(StackableComponent* stack)
{   // returns true if stack count changes
    int room = this->max - this->count;
    if (room <= 0) return false;
    int available = stack->count;
    if (available <= 0) return false;
    if (room < available) available = room;
    this->count += available;
    stack->count -= available;
    return true;
}

} // Components
