#include "stackable.hpp"

namespace Components
{

bool StackableComponent::merge_stack(StackableComponent* stack)
{   // returns true if stack size changes
    int room = this->max - this->size;
    if (room <= 0) return false;
    int available = stack->size;
    if (available <= 0) return false;
    if (room < available) available = room;
    this->size += available;
    stack->size -= available;
    return true;
}

} // Components
