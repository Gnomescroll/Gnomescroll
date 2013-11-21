/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

namespace Particle
{

class BillboardSprite
{
    public:
        float scale;

        #if DC_CLIENT
        int texture_index;
        void draw(Vec3 v);
        void draw_from_bottom(Vec3 v);
        #endif

        BillboardSprite();
};

}   // Particle
