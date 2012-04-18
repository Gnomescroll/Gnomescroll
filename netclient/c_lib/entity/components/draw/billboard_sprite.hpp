#pragma once

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/components/draw.hpp>

namespace Components
{

class BillboardSpriteComponent: public DrawComponent
{
    public:
        int sprite_index;
        float scale;
        
        void call();

    BillboardSpriteComponent()
    : DrawComponent(COMPONENT_BILLBOARD_SPRITE),
    sprite_index(4), scale(1.0f)
    {}

};
    
} // Components
