#pragma once

#include <item/data/enum.hpp>
#include <item/data/constant.hpp>

namespace item
{

void init_container(class ItemContainer* container, ItemContainerType type);

#if DC_SERVER
//  tell client to assign container to an agent
void send_container_assign(class ItemContainer* container, int client_id);
void send_container_create(class ItemContainer* container, int client_id);
void send_container_delete(class ItemContainer* container, int client_id);
#endif

typedef int ItemID;
const int EMPTY_SLOT = 0xffff;      // value of empty slot
const int NULL_SLOT = -1;           // invalid slot number

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

        bool is_full()
        {
            assert(this->slot_count <= this->slot_max);
            assert(this->slot_count >= 0);
            return (this->slot_count >= this->slot_max);
        }

        bool is_valid_grid_position(int slot)
        {
            return (slot >= 0 && slot < this->slot_max);
        }

        int get_empty_slot()
        {
            for(int i=0; i<this->slot_max; i++)
                if (this->slot[i] == EMPTY_SLOT)
                    return i;
            return NULL_SLOT;
        }

        void insert_item(int item_id, int slot)
        {
            this->slot[slot] = item_id;
            this->slot_count++;
        }

        void remove_item(int slot)
        {
            this->slot[slot] = EMPTY_SLOT;
        }

        /* initializers */

        void init(ItemContainerType type, int xdim, int ydim)
        {
            this->type = type;
            this->xdim = xdim;
            this->ydim = ydim;
            this->slot_max = xdim*ydim;
            this->slot = new ItemID[this->slot_max];
            for(int i=0; i<this->slot_max; this->slot[i++] = EMPTY_SLOT);
        }

        ~ItemContainer()
        {
           if (this->slot != NULL) delete[] this->slot;
        }

        ItemContainer(int id)
        : id(id), type(CONTAINER_TYPE_NONE),
        xdim(0), ydim(0),
        slot_max(0), slot_count(0), slot(NULL)
        {}
};

}

#include <common/template/object_list.hpp>

namespace item
{

const int ITEM_CONTAINER_MAX = 1024;

class ItemContainerList: public Object_list<ItemContainer, ITEM_CONTAINER_MAX>
{
    private:
        const char* name() { return "ItemContainer"; }
    public:
        ItemContainerList() { print_list((char*)this->name(), this); }
};

}
