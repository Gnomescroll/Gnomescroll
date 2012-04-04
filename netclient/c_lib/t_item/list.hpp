#pragma once

#include <c_lib/t_item/free_item.hpp>

namespace t_item
{

const int FREE_ITEM_MAX = 1024;

class Free_item_list: public Object_list<Free_item, FREE_ITEM_MAX>
{
    private:
        const char* name() { return "Free_item"; }
    public:
        Free_item_list() { print(); }

        void draw();
        void tick();
};

void Free_item_list::draw()
{
    for (int i=0; i<this->n_max; i++)
        if (this->a[i] != NULL)
            this->a[i]->draw();
}

void Free_item_list::tick()
{
    Free_item* free_item;
    for (int i=0; i<this->n_max; i++)
    {
        free_item = this->a[i];
        if (free_item == NULL) continue;
        free_item->tick();
        if (free_item->ttl >= free_item->ttl_max)
            this->destroy(free_item->id);
    }
}


}
