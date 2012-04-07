#include "list.hpp"

void OwnedList::transfer_ownership(int owner, int new_owner)
{
    #if DC_SERVER
    if (owner == new_owner)
    {
        printf("WARNING -- %s_list::transfer_ownership -- owner == new_owner %d\n", this->name(), owner);
        return;
    }
    OwnedProperties* state;
    if (this->ct == 0) return;
    for (int i=0; i<this->max; i++)
    {
        state = (OwnedProperties*)this->objects[i];
        if (state == NULL) continue;
        if (state->owner != owner) continue;
        state->obj->set_owner(new_owner); // TODO
        alter_item_ownership_StoC msg;
        msg.owner = new_owner;
        msg.id = state->obj->state()->id;
        msg.type = state->obj->state()->type;
        msg.broadcast();
    }
    #endif
}
