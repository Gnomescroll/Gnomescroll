#include "textures.hpp"

#ifdef DC_CLIENT

#include <compat_gl.h>
#include <c_lib/camera/camera.hpp>
#include <c_lib/SDL/texture_loader.hpp>
#include <c_lib/SDL/draw_functions.hpp>

static int particle_texture;

int init_particles() {
    int i = create_texture_from_file((char*) "./media/texture/particles_01.png", &particle_texture);
    if (i)
    {
        printf("init_particles failed with code %d\n", i);
        return 1;
    }
    return 0;
}

void BillboardSprite::draw(Vec3 v)
{
    if (current_camera == NULL
    || !current_camera->in_view(v.x, v.y, v.z))
        return;

    Vec3 up = vec3_init(
        model_view_matrix[0]*this->scale,
        model_view_matrix[4]*this->scale,
        model_view_matrix[8]*this->scale
    );
    Vec3 right = vec3_init(
        model_view_matrix[1]*this->scale,
        model_view_matrix[5]*this->scale,
        model_view_matrix[9]*this->scale
    );

    float tx_min, tx_max, ty_min, ty_max;
    tx_min = (float)(this->texture_index%16)* (1.0/16.0);
    tx_max = tx_min + (1.0/16.0);
    ty_min = (float)(this->texture_index/16)* (1.0/16.0);
    ty_max = ty_min + (1.0/16.0);

    Vec3 p = vec3_sub(v, vec3_add(right, up));    // Bottom left
    glTexCoord2f(tx_min,ty_max);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(v, vec3_sub(up, right));         // Top left
    glTexCoord2f(tx_min,ty_min);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(v, vec3_add(up, right));         // Top right
    glTexCoord2f(tx_max,ty_min);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(v, vec3_sub(right, up));         // Bottom right
    glTexCoord2f(tx_max,ty_max);
    glVertex3f(p.x, p.y, p.z);
}

BillboardSprite::BillboardSprite()
:
scale(1.0f),
texture_index(0)
{}

void begin_transparent_particle_draw()
{
    begin_transparent_draw(particle_texture);
}
void end_transparent_particle_draw()
{
    end_transparent_draw();
}

#endif
