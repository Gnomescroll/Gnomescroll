#include "hitscan.hpp"

#if DC_SERVER
dont_include_this_file_in_server
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
        float vx = vm*(float)rand()/(float)RAND_MAX;
        float vy = 160.0f;
        float vz = vm*(float)rand()/(float)RAND_MAX;
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

inline void RailRayEffect::reset()
{
    this->ttl = HITSCAN_TTL;
}

void RailRayEffect::tick()
{
    //const float tick_rate = 1.0f / 30.0f;
    //this->start = vec3_add(this->start, vec3_scalar_mult(this->end, tick_rate));
    //this->start = translate_position(this->start);

    // spin particles
}

void RailRayEffect::draw(Vec3 camera)
{
    if (Options::animation_level <= 0) return;

    static const float span = 0.9f; // space between particle layers
    static const float spin_span = PI / 8;

    float dist = vec3_distance(this->end, this->start);
    float step = span / dist;
    Vec3 fwd = vec3_sub(this->end, this->start);


    // RIGHT SETTINGS WOULD LOOK GOOD ON MOB IMPACTS ---> voxel_explode(this->end, /*min*/ 4, /*max*/ 14, /*size*/ 0.2f, /*force*/ 0.1f, COLOR_GREEN);

    float theta, phi;
    vec3_to_angles(fwd, &theta, &phi);
    printf("fwd   theta: %9.3f  phi: %9.3f \n", theta, phi);

    float curr_spin = 0.0f;
    for (float fl=0.0f; fl<=1.0f; fl+=step) 
    {
        Vec3 curr = vec3_interpolate(this->start, this->end, fl);
        float r = 0.25f; // quadratic radius
        
        Vec3 spiral = vec3_init(
            r * cosf(curr_spin),
            0,
            r * sinf(curr_spin)
        );
        Vec3 spiral2 = vec3_init(
            r * cosf(curr_spin + PI),
            0,
            r * sinf(curr_spin + PI)
        );

        spiral = vec3_euler_rotation(spiral, 0, 0, theta-0.5f);
        spiral2 = vec3_euler_rotation(spiral2, 0, 0, theta-0.5f);
        spiral = vec3_add(curr, spiral);
        spiral2 = vec3_add(curr, spiral2);
        


        ////float anim_scale = float(Options::animation_level)/3.0f;
        ////n = anim_scale*float(n);
        //Particle::Shrapnel *s;

        //s = Particle::create_shrapnel(spiral, /*vel*/ vec3_init(0,0,0)/*vec3_rand_center()*/);
        //if (s==NULL) return;

        //s->ttl = randrange(8,15);
        //s->scale = 0.1f;
        //s->texture_index = 54;

        //s = Particle::create_shrapnel(spiral2, /*vel*/ vec3_init(0,0,0)/*vec3_rand_center()*/);
        //if (s==NULL) return;

        //s->ttl = randrange(8,15);
        //s->scale = 0.1f;
        //s->texture_index = 54;



        draw_quad(curr, r, theta, phi);

        curr_spin += spin_span;
        if (curr_spin >= PI*2)
            curr_spin = 0.0f;
    }
}

void RailRayEffect::draw_quad(Vec3 p, float r, float theta, float phi) // quadratic radius
{ // with no rotation modifications, it faces upwards
    static const float tx_min = 0.0f;
    static const float tx_max = 1.0f;
    static const float ty_min = 0.0f;
    static const float ty_max = 1.0f;

    r *= 5.0f;

    Vec3 bl, tl, tr, br;
    bl = tl = tr = br = vec3_init(0.0f);

    //Vec3 bl = vec3_init(-r, -r/3, 0);  // Bottom left
    //Vec3 tl = vec3_init(-r,  r/3, 0);  // Top left
    //Vec3 tr = vec3_init( r,  r/3, 0);  // Top right
    //Vec3 br = vec3_init( r, -r/3, 0);  // Bottom right


    // MOVE SHIT LIKE THIS TO BE CALC'ED ONLY ONCE then applied to all the interpolated points
    // rotate the y & z (pitch) .... prob need to reverse/mirror the y
    float radian = phi*PI;

    // bottom
    bl.y = br.y = -r*sinf(radian);
    bl.z = br.z = r*cosf(radian);
    printf("bl.y: %8.2f  bl.z: %8.2f \n", bl.y, bl.z);
    //printf("radian: %8.2f \n", radian);
    
    // top
    tl.y = tr.y = r*sinf(radian);
    tl.z = tr.z = -r*cosf(radian);
    printf("tl.y: %8.2f  tl.z: %8.2f \n", tl.y, tl.z);

    GS_ASSERT(bl.y == br.y);
    GS_ASSERT(bl.z == br.z);
    GS_ASSERT(tl.y == tr.y);
    GS_ASSERT(tl.z == tr.z);
    
    //bl.y = br.y = -r;//*sinf(radian);
    //bl.z = br.z = 0;//*cosf(radian);
    //
    //tl.y = tr.y = r;//*sinf(radian);
    //tl.z = tr.z = 0;//*cosf(radian);
    
    bl.x = tl.x = -r;
    br.x = tr.x = r;
    //bl = vec3_euler_rotation(bl, theta, 0, 0);
    //tl = vec3_euler_rotation(tl, theta, 0, 0);
    //tr = vec3_euler_rotation(tr, theta, 0, 0);
    //br = vec3_euler_rotation(br, theta, 0, 0);
    bl = vec3_add(p, bl);
    tl = vec3_add(p, tl);
    tr = vec3_add(p, tr);
    br = vec3_add(p, br);

    glTexCoord2f(tx_max, ty_max);
    glVertex3f(bl.x, bl.y, bl.z);  // Bottom left
    glTexCoord2f(tx_min, ty_max);
    glVertex3f(tl.x, tl.y, tl.z);  // Top left
    glTexCoord2f(tx_min, ty_min);
    glVertex3f(tr.x, tr.y, tr.z);  // Top right
    glTexCoord2f(tx_max, ty_min);
    glVertex3f(br.x, br.y, br.z);  // Bottom right

    //glTexCoord2f(tx_max, ty_max);
    //glVertex3f(p.x-r, p.y, p.z-r/3);  // Bottom left
    //glTexCoord2f(tx_min, ty_max);
    //glVertex3f(p.x-r, p.y, p.z+r/3);  // Top left
    //glTexCoord2f(tx_min, ty_min);
    //glVertex3f(p.x+r, p.y, p.z+r/3);  // Top right
    //glTexCoord2f(tx_max, ty_min);
    //glVertex3f(p.x+r, p.y, p.z-r/3);  // Bottom right
}

void RailRayEffectList::draw()
{
    IF_ASSERT(current_camera == NULL) return;
    if (this->num == 0) return;

    //_LAST_TICK();
    //_GET_MS_TIME();

    glColor3ub(255,255,255);

    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_DEPTH_WRITEMASK, false);
    GL_ASSERT(GL_BLEND, true);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, hitscan_texture_id);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);

    glBegin(GL_QUADS);

    for (size_t i=0; i<this->num; i++)
        a[i].draw(current_camera->get_position());

    glEnd();
    glDisable(GL_TEXTURE_2D);

}

void RailRayEffectList::tick()
{
    for (size_t i=0; i<this->num; i++)
    {
        a[i].tick();
        a[i].ttl--;
        if (a[i].ttl <= 0)
            destroy(i);
    }
}

}   // Animations