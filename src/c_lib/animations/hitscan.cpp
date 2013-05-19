#include "hitscan.hpp"

#if DC_SERVER
# error Do not include this file in the server
#endif

#include <state/client_state.hpp>
#include <stdlib.h>

#include <camera/camera.hpp>

#include <common/compat_gl.h>
#include <SDL/texture_loader.hpp>

namespace Animations
{

const int HITSCAN_TTL = 5;
static GLuint hitscan_texture_id = 0;

void init_hitscan()
{
    int ret = create_texture_from_file(MEDIA_PATH "sprites/hitscan/hitscan_01.png", &hitscan_texture_id);
    GS_ASSERT(ret == 0);
}

void teardown_hitscan()
{

}

inline void HitscanEffect::reset()
{
    this->ttl = HITSCAN_TTL;
}

// DO NOT USE:: TESTING PURPOSES ONLY
void HitscanEffect::add_plane_bias()
{
    // biases the laser effect in a circle oriented along the normal plane

    // take cross product of up and look vectors
    // assume vx,vy,vz to be the look vector
    struct Vec3 look;
    IF_ASSERT(vec3_length_squared(this->v) == 0.0f) look = vec3_init(1,0,0);
    look = vec3_normalize(this->v);
    Vec3 up = vec3_init(0,0,1);
    Vec3 right = vec3_cross(look, up);
    right = vec3_normalize(right);

    //// this method biases the laser to a random point in circle
    //float negxy = (rand()%2 == 0) ? -1 : 1;
    //float negz = (rand()%2 == 0) ? -1 : 1;

    //const float floor_ = 0.15;
    //const float ceil_ = 0.15;

    //float dxy = (randf() * (ceil_ - floor_)) + floor_;
    //float dz = (randf() * (ceil_ - floor_)) + floor_;

    //this->x += dxy * negxy * right.x;
    //this->y += dxy * negxy * right.y;
    //this->z += dz * negz;


    //// this method uses time to move the bias in a circle
    //const float rot_speed = 3000.0f;
    //float theta = ((float)_get_ticks())/rot_speed;
    //const float r = 0.19f;

    //float dxy = r * cos(theta);
    //float dz = r * sin(theta);
    //this->x += dxy * right.x;
    //this->y += dxy * right.y;
    //this->z += dz;

    //printf("dxy=%0.2f dz=%0.2f\n", dxy, dz);

     //fixed in the bottom right corner
    const float dxy = 0.14f;
    const float dz = -0.13f;
    right.z = 1.0f;
    this->p = vec3_add(this->p, vec3_mult(vec3_init(dxy, dxy, dz), right));
}

void HitscanEffect::tick()
{
    const float tick_rate = 1.0f / 30.0f;
    this->p = vec3_add(this->p, vec3_scalar_mult(this->v, tick_rate));
    this->p = translate_position(this->p);
}

void HitscanEffect::draw(Vec3 camera)
{
    const float width = 0.50f;
    const float height = 0.25f;  // length per velocity

    struct Vec3 position = quadrant_translate_position(camera, this->p);

    // Disabled for lasers, point test too inaccurate
    //if (!point_fulstrum_test(x,y,z)) return;

    struct Vec3 v = vec3_normalize(this->v);
    Vec3 look = vec3_sub(position, camera);

    Vec3 u1 = vec3_cross(look, v);
    u1 = vec3_normalize(u1);

    Vec3 x2 = vec3_sub(position, vec3_scalar_mult(this->v, height));
    look = vec3_sub(x2, camera);

    Vec3 u2 = vec3_cross(look, this->v);
    u2 = vec3_normalize(u2);

    u1 = vec3_scalar_mult(u1, width);
    u2 = vec3_scalar_mult(u2, width);

    static const float tx_min = 0.0f;
    static const float tx_max = 1.0f;
    static const float ty_min = 0.0f;
    static const float ty_max = 1.0f;

    glTexCoord2f(tx_max, ty_max);
    glVertex3f(x2.x + u2.x, x2.y + u2.y, x2.z + u2.z);  // Bottom left

    glTexCoord2f(tx_min, ty_max);
    glVertex3f(position.x + u1.x, position.y + u1.y, position.z + u1.z);  // Top left

    glTexCoord2f(tx_min,ty_min);
    glVertex3f(position.x - u1.x, position.y - u1.y, position.z - u1.z);  // Top right

    glTexCoord2f(tx_max,ty_min);
    glVertex3f(x2.x - u2.x, x2.y - u2.y, x2.z - u2.z);  // Bottom right
}

void HitscanEffectList::draw()
{
    IF_ASSERT(current_camera == NULL) return;
    if (this->num == 0) return;

    //_LAST_TICK();
    //_GET_MS_TIME();

    Vec3 camera = current_camera->get_position();

    glColor3ub(255,255,255);

    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_DEPTH_WRITEMASK, false);
    GL_ASSERT(GL_BLEND, true);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, hitscan_texture_id);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);

    glBegin(GL_QUADS);

    for (size_t i=0; i<this->num; i++)
        a[i].draw(camera);

    glEnd();
    glDisable(GL_TEXTURE_2D);

}

void HitscanEffectList::tick()
{
    #define _HE_DEBUG 0
    #if _HE_DEBUG
    static int frame = 0;
    frame++;

    if (frame % 8 == 0)
    {
        const float vm = 16.0f;
        float vx = vm*randf();
        float vy = 160.0f;
        float vz = vm*randf();
        HitscanEffect* he = hitscan_effect_list->create();
        he->set_state(vec3_init(32.0f, 32.0f, 64.0f), vec3_init(vx, vy, vz));
    }
    #endif
    #undef _HE_DEBUG

    for (size_t i=0; i<this->num; i++)
    {
        a[i].tick();
        a[i].ttl--;
        if (a[i].ttl <= 0)
            destroy(i);
    }
}

}   // Animations
