#pragma once

#include <c_lib/entity/policy.hpp>
#include <c_lib/entity/layers.hpp>

class Inventory: public ObjectInterfacePlaceholder
{
    private:
        int get_slot(int x, int y)
        {
            return this->x*y + x;
        }

        int get_empty_slot()
        {
            if (this->full())
                return -1;
            for (int i=0; i<this->max; i++)
                if (this->contents[i] == NULL)
                    return i;
            return -1;
        }

        bool is_valid_grid_position(int x, int y)
        {
            if (x < 0 || x >= this->x || y < 0 || y >= this->y)
                return false;
            return true;
        }

    public:
        int id;
        Object_types type;  // todo: bag subtype
        int x,y;
        ObjectPolicyInterface** contents;

        int max;
        int ct;

        int owner;
        
    void init(int x, int y)
    {
        if (!x || !y)
        {
            printf("WARNING:: Inventory::init() -- dimension is 0: x,y = %d,%d\n", x,y);
            return;
        }
        this->x = x;
        this->y = y;
        this->max = x*y;
        this->contents = (ObjectPolicyInterface**)calloc(this->max, sizeof(ObjectPolicyInterface*));
    }

    bool type_allowed(Object_types type)
    {   // Restrict types here
        return true;
    }

    bool full()
    {
        if (this->ct >= this->max)
            return true;
        return false;
    }

    bool add(ObjectPolicyInterface* obj, int x, int y)
    {
        if (!this->type_allowed(obj->state()->type))
            return false;
        if (this->full())
            return false;

        if (!this->is_valid_grid_position(x,y))
            return NULL;
        int slot = this->get_slot(x,y);
        if (this->contents[slot] != NULL)
            return false;

        this->contents[slot] = obj;
        this->ct++;
        return true;
    }

    // auto assign slot
    // TODO: for stackable items, put on stack
    bool add(ObjectPolicyInterface* obj)
    {
        if (!this->type_allowed(obj->state()->type))
            return false;
        int slot = this->get_empty_slot();
        if (slot < 0)
            return false;
        this->contents[slot] = obj;
        this->ct++;
        return true;
    }

    ObjectPolicyInterface* remove(int x, int y)
    {
        ObjectPolicyInterface* obj = this->item_at_slot(x,y);
        int slot = this->get_slot(x,y);
        if (slot >= 0)
            this->contents[slot] = NULL;
        return obj;
    }

    ObjectPolicyInterface* item_at_slot(int x, int y)
    {
        if (!this->is_valid_grid_position(x,y))
            return NULL;
        int slot = this->get_slot(x,y);
        return this->contents[slot];
    }

    explicit Inventory(int id)
    :
    id(id),
    type(OBJ_TYPE_INVENTORY),
    x(0),y(0),
    contents(NULL),
    max(0), ct(0),
    owner(NO_AGENT)
    {
    }

    ~Inventory()
    {
        if (this->contents != NULL)
            free(this->contents);
    }

};
