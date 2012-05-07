#pragma once

#include <item/common/enum.hpp>
#include <item/common/constant.hpp>

namespace Item
{

// init
void init_container(class ItemContainer* container, ItemContainerType type);

#if DC_CLIENT
// transactions
ContainerActionType alpha_action_decision_tree(int id, int slot);
ContainerActionType beta_action_decision_tree(int id, int slot);
#endif

#if DC_SERVER
// transactions
ContainerActionType alpha_action_decision_tree(int agent_id, int client_id, int id, int slot);
ContainerActionType beta_action_decision_tree(int agent_id, int client_id, int id, int slot);

//network
//  tell client to assign container to an agent
void send_container_assign(class ItemContainer* container, int client_id);
void send_container_create(class ItemContainer* container, int client_id);
void send_container_delete(class ItemContainer* container, int client_id);
#endif

class ItemContainer
{
    public:
        int id;
        ItemContainerType type;

        int xdim;
        int ydim;

        int slot_max;
        int slot_count;
        ItemID* slot;

        int owner;

        bool is_full()
        {
            assert(this->slot_count <= this->slot_max && this->slot_count >= 0);
            return (this->slot_count >= this->slot_max);
        }

        bool is_valid_slot(int slot)
        {
            return (slot >= 0 && slot < this->slot_max);
        }

        int get_stackable_slot(int item_type, int stack_size)
        {
            for (int i=0; i<this->slot_max; i++)
            {
                if (this->slot[i] == NULL_ITEM) continue;
                if (get_item_type(this->slot[i]) == item_type   // stacks
                && get_stack_space(this->slot[i]) >= stack_size) // stack will fit
                    return i;
            }
            return NULL_SLOT;
        }

        int get_empty_slot()
        {
            for (int i=0; i<this->slot_max; i++)
                if (this->slot[i] == NULL_ITEM)
                    return i;
            return NULL_SLOT;
        }

        ItemID get_item(int slot)
        {
            assert(this->is_valid_slot(slot));
            return this->slot[slot];
        }

        void insert_item(int slot, ItemID item_id);
        void remove_item(int slot);

        /* initializers */

        void init(ItemContainerType type, int xdim, int ydim)
        {
            this->type = type;
            this->xdim = xdim;
            this->ydim = ydim;
            this->slot_max = xdim*ydim;
            assert(this->slot_max < NULL_SLOT);
            this->slot = new ItemID[this->slot_max];
            for (int i=0; i<this->slot_max; this->slot[i++] = NULL_ITEM);
        }
        
        void init(ItemContainerType type, int xdim, int ydim, int extra)
        {
            this->type = type;
            this->xdim = xdim;
            this->ydim = ydim;
            this->slot_max = xdim*ydim + extra;
            assert(this->slot_max < NULL_SLOT);
            this->slot = new ItemID[this->slot_max];
            for (int i=0; i<this->slot_max; this->slot[i++] = NULL_ITEM);
        }

        void assign_owner(int owner)
        {
            this->owner = owner;
        }

        ~ItemContainer()
        {
           if (this->slot != NULL) delete[] this->slot;
        }

        ItemContainer(int id)
        : id(id), type(CONTAINER_TYPE_NONE),
        xdim(0), ydim(0),
        slot_max(0), slot_count(0), slot(NULL),
        owner(NO_AGENT)
        {}
};

}

#include <common/template/object_list.hpp>

namespace Item
{

const int ITEM_CONTAINER_MAX = 1024;

class ItemContainerList: public Object_list<ItemContainer, ITEM_CONTAINER_MAX>
{
    private:
        const char* name() { return "ItemContainer"; }
    public:
        ItemContainerList() { print_list((char*)this->name(), this); }

        #if DC_CLIENT
        ItemContainer* create()
        {
            printf("must create item container with id\n");
            assert(false);
            return NULL;
        }

        ItemContainer* create(int id)
        {
            return Object_list<ItemContainer, ITEM_CONTAINER_MAX>::create(id);
        }
        #endif
};

}
