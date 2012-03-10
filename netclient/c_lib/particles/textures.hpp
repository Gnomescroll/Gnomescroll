#pragma once

#ifdef DC_CLIENT

int init_particles();

class BillboardSprite
{
    public:
        float scale;
        int texture_index;

        void draw(Vec3 v);

        BillboardSprite();
};

#endif
