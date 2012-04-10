#pragma once

#include <c_lib/objects/common/list/list.hpp>
#include <c_lib/objects/components/sprite/sprite.hpp>

const int sprite_LIST_MAX = 1024;

class SpriteList: public BehaviourList
{
    private:
        const char* name() { return "SpriteList"; }
    public:
        void draw()
        {
            if (this->ct == 0) return;
            for (int i=0; i<this->max; i++)
                if (this->objects[i] != NULL && this->objects[i]->obj != NULL)
                    this->objects[i]->obj->draw();
        }

    SpriteList()
    : BehaviourList(sprite_LIST_MAX)
    {}
};