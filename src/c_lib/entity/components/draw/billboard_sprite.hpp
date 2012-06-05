#pragma once

#if DC_CLIENT

#include <entity/constants.hpp>
#include <entity/components/draw.hpp>

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
    sprite_index(0), scale(1.0f)
    {}

};
    
} // Components

#endif
