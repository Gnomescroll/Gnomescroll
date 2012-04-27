#pragma once

#if DC_SERVER

namespace Item
{

//bool auto_add_container_item(ItemContainer* container, int slot, int id)
//{
    //int slot = container->get_slot_for_stack(type, stack_size);

    //if (container->is_valid_slot(slot))
    //{   // add to a stack
        //container->add_stack(slot, stack_size);
        //stack_size = container->objects[slot].count;
        //if (owner != NO_AGENT)
            //container->sendToClientSetStack(slot, stack_size);
        //else
            //container->broadcastSetStack(slot, stack_size);
    //}
    //else
    //{   // add normally
        //slot = container->get_empty_slot();
        //if (slot < 0) return false;
        //container->add(id, type, stack_size, slot);
        //if (owner != NO_AGENT)
            //container->sendToClientAdd(id, type, stack_size, slot);
        //else
            //container->broadcastAdd(id, type, stack_size, slot);
    //}
    //return true;
//}

//bool add_container_item(ItemContainer* container, int slot, int id)
//{
    //bool added = container->add(id, type, stack_size, slot);
    //if (!added) return false;
    //if (owner != NO_AGENT)
        //container->sendToClientAdd(id, type, stack_size, slot);
    //else
        //container->broadcastAdd(id, type, stack_size, slot);
    //return true;
//}

//bool remove_container_item(ItemContainer* container, int slot)
//{
    //bool removed = container->remove(slot);
    //if (!removed) return false;
    //if (owner != NO_AGENT)
        //container->sendToClientRemove(slot);
    //else
        //container->broadcastRemove(slot);
    //return true;
//}

//// dumps contents
//void remove_all_container_items(ItemContainer* container);

//bool swap_container_item(ItemContainer* container, int slota, int slotb)
//{
    //bool swapped = container->swap(slota, slotb);
    //if (owner != NO_AGENT)
        //container->sendToClientSwap(slota, slotb);
    //else
        //container->broadcastSwap(slota, slotb);
    //return swapped;
//}

//bool merge_container_stack(ItemContainer* container, int slota, int slotb)
//{
    //bool merged = container->merge_stack(slota, slotb, count);
    //if (owner != NO_AGENT)
        //container->sendToClientMergeStack(slota, slotb, count);
    //else
        //container->broadcastMergeStack(slota, slotb, count);
    //return merged;
//}

}

#endif
