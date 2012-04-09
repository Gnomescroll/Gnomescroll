#pragma once

class BillboardSprite
{
    public:
        float scale;
        int texture_index;

        void draw(Vec3 v);
        void draw_from_bottom(Vec3 v);

        BillboardSprite();
};
