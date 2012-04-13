#pragma once

#if DC_CLIENT

#include <c_lib/objects/common/list/list.hpp>
#include <c_lib/objects/components/sprite/sprite.hpp>

const int SPRITE_LIST_MAX = 1024;

class SpriteList: public BehaviourList
{
    private:
        const char* name() { return "SpriteList"; }
    public:
        void draw();

    SpriteList()
    : BehaviourList(SPRITE_LIST_MAX)
    {}
};

#endif
