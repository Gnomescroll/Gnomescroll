#pragma once

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/components/draw.hpp>

namespace Components
{

class BillboardSpriteComponent: public DrawComponent
{
    public:
        void draw() {}

    BillboardSpriteComponent()
    : DrawComponent(COMPONENT_BILLBOARD_SPRITE)
    {}

};
    
} // Components
