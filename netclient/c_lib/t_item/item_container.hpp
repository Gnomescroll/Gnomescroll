#pragma once


namespace t_item
{


typedef int ItemId;

const int EMPTY_SLOT = 0xffff;

typedef enum
{
    INVENTORY_AGENT,
    //INVENTORY_BLOCK,
    //INVENTORY_BAG,
} ItemContainerType;

class ItemContainer // dont use behaviour list unless doing the registration model
{
    public:

        int id;

        int xdim;
        int ydim;

        int slot_max;
        int container_type;

        ItemId* slot;

        ItemContainer(int _id)
        {
            id = _id;
            slot = NULL;
        }

        void init(ItemContainerType type, int _xdim, int _ydim)
        {
            container_type = type;

            xdim = _xdim;
            ydim = _ydim;

            slot_max = xdim*ydim;

            slot = new ItemId[slot_max];
            for(int i=0; i<slot_max; i++) slot[i] = EMPTY_SLOT;
            //printf("EMPTY_SLOT = %i \n", EMPTY_SLOT);
        }

        void init_agent_inventory()
        {
            this->init(INVENTORY_AGENT, 18, 12);
        }

        ~ItemContainer()
        {
           delete[] slot;
        }

        bool is_full()
        {
            //printf("EMPTY_SLOT = %i \n", EMPTY_SLOT);
            for(int i=0; i<slot_max; i++) 
            {
                //printf("slot %i = %i \n", i, slot[i]);
                if(slot[i] == EMPTY_SLOT) 
                    return false; 
            }
            return true;
        }

        bool is_valid_grid_position(int x, int y)
        {
            return (x < 0 || x >= xdim || y < 0 || y >= ydim) ? false : true;
        }

        bool is_valid_grid_position(int _slot)
        {
            int x = _slot % xdim;
            int y = _slot / xdim;
            return (x < 0 || x >= xdim || y < 0 || y >= ydim) ? false : true;
        }

        int get_empty_slot()
        {
            for(int i=0; i<slot_max; i++) { if(slot[i] == EMPTY_SLOT) return i; }
            return -1;
        }

        void insert_item(int item_id, int x, int y)
        {
            assert(is_valid_grid_position(x,y));
            slot[y*xdim + x] = item_id;
            printf("inserted item %d at %d,%d in inventory %d \n", item_id, x, y, id);
        }

        //create item, return slot
        int create_item(int item_id)
        {
            int _slot = get_empty_slot();
            int x = _slot % xdim;
            int y = _slot / xdim;
            assert(is_valid_grid_position(x,y));
            slot[y*xdim + x] = item_id;
            printf("inserted item %d at %d,%d in inventory %d \n", item_id, x, y, id);

            return _slot;
        }

        void clear_slot(int x, int y)
        {
            assert(is_valid_grid_position(x,y));
            slot[y*xdim + x] = EMPTY_SLOT;
        }
/*
        void sendToClient(int inventory_id, int client_id);
*/
};

 
}

#include <c_lib/template/object_list.hpp>

namespace t_item
{

const int ITEM_CONTAINER_MAX = 1024;

class ItemContainerList: public Object_list<ItemContainer, ITEM_CONTAINER_MAX>
{
    private:
        const char* name() { return "ItemContainer"; }
    public:
        ItemContainerList() { print(); }
};

}
