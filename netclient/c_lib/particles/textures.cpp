#include "textures.hpp"


#include <compat_gl.h>
#include <c_lib/camera/camera.hpp>
#include <c_lib/SDL/texture_loader.hpp>
#include <c_lib/SDL/draw_functions.hpp>

namespace Particles
{

GLuint particle_texture = 0;

void init_for_draw() 
{
    int i = create_texture_from_file((char*) "./media/texture/particles_01.png", &particle_texture);
    if (i)
    {
        printf("Particles::init_for_draw failed with code %d\n", i);
    }

    //particle_texture = i;
}

void draw_teardown()
{


}

void begin_particle_draw()
{
    assert(particle_texture != 0);
    GL_ASSERT(GL_TEXTURE_2D, true);

    glColor4ub(255,255,255,255);
    glDepthMask(GL_FALSE);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBindTexture(GL_TEXTURE_2D, particle_texture);
    glBegin(GL_QUADS);
}
void end_particle_draw()
{
    glEnd();
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}

}
