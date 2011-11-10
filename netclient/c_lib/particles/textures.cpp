#include "textures.hpp"

#ifdef DC_CLIENT

static int particle_texture_id;

int init_particles() {
    int i;
    i = create_texture_from_file("./media/texture/particles_01.png", &particle_texture_id);
    if (i) {
        printf("init_particles failed with code %d\n", i);
        return 1;
    }
    return 0;
}

#endif
