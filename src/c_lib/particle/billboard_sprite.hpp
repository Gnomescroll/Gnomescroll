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
