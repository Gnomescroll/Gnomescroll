#pragma once

#include <c_lib/objects/common/list/list.hpp>
#include <c_lib/objects/components/billboard_sprite/billboard_sprite.hpp>

const int BILLBOARD_SPRITE_LIST_MAX = 1024;

class BillboardSpriteList: public BehaviourList
{
    private:
        const char* name() { return "BillboardSpriteList"; }
    public:
        void draw()
        {
            if (this->ct == 0) return;
            for (int i=0; i<this->max; i++)
                if (this->objects[i] != NULL && this->objects[i]->obj != NULL)
                    this->objects[i]->obj->draw();
        }

    BillboardSpriteList()
    : BehaviourList(BILLBOARD_SPRITE_LIST_MAX)
    {}
};
