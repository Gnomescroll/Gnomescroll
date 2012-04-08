#pragma once

#if DC_CLIENT
int init_particles();
void begin_particle_draw();
void end_particle_draw();
#endif

class BillboardSprite
{
    public:
        float scale;
        int texture_index;

        void draw(Vec3 v);
        void draw_from_bottom(Vec3 v);

        BillboardSprite();
};
