#pragma once

namespace Particle
{

#if DC_CLIENT
class BillboardSprite
{
    public:
        float scale;
        int texture_index;

        void draw(Vec3 v);
        void draw_from_bottom(Vec3 v);

        BillboardSprite();
};
#else
class BillboardSprite
{
    public:
        float scale;
        int texture_index;

        void draw(Vec3 v) {};
        void draw_from_bottom(Vec3 v) {};

        BillboardSprite() {};
};
#endif
}
