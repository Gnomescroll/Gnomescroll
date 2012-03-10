#pragma once

#ifdef DC_CLIENT

int init_particles();
void begin_transparent_particle_draw();
void end_transparent_particle_draw();

class BillboardSprite
{
    public:
        float scale;
        int texture_index;

        void draw(Vec3 v);

        BillboardSprite();
};

#endif
