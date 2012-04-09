#include "textures.hpp"

#if DC_CLIENT

#include <compat_gl.h>
#include <c_lib/camera/camera.hpp>
#include <c_lib/SDL/texture_loader.hpp>
#include <c_lib/SDL/draw_functions.hpp>

static GLuint particle_texture;

int init_particles() {
    int i = create_texture_from_file((char*) "./media/texture/particles_01.png", &particle_texture);
    if (i)
    {
        printf("init_particles failed with code %d\n", i);
        return 1;
    }
    return 0;
}

void begin_particle_draw()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBindTexture(GL_TEXTURE_2D, particle_texture);
    glBegin(GL_QUADS);
}
void end_particle_draw()
{
    glEnd();
    glDisable(GL_BLEND);
}

#endif