#include "hitscan_laser.hpp"

#include <state/client_state.hpp>
#include <stdlib.h>

#include <camera/camera.hpp>

#include <common/compat_gl.h>
#include <SDL/texture_loader.hpp>

/*
 *
 * NOT USED. go to hitscan.cpp
 *
 */

namespace Animations
{

static GLuint hitscan_laser_texture_00_id;
static GLuint hitscan_laser_texture_01_id;

void init_hitscan_laser()
{
    int i;
    i = create_texture_from_file( (char*) "./media/texture/hitscan/laser_beam_00.png", &hitscan_laser_texture_00_id);
    if (i) { printf("init_hitscan_laser failed with code %d\n", i); }
    i = create_texture_from_file( (char*) "./media/texture/hitscan/laser_beam_core_00.png", &hitscan_laser_texture_01_id);
    if (i) { printf("init_hitscan_laser failed with code %d\n", i); }
}

void teardown_hitscan_laser()
{

}

void HitscanLaserEffect::init()
{
    this->ttl = HITSCAN_LASER_TTL;
}

void HitscanLaserEffect::tick()
{
    //check for collision with terrain/players
    //play animations for terrain/player collision
}

void HitscanLaserEffect::draw1(float delta, Vec3 camera)
{
    const float width = 0.5;

    float x = quadrant_translate_f(camera.x, this->x);
    float y = quadrant_translate_f(camera.y, this->y);
    float z = this->z;
    
    float fx = quadrant_translate_f(camera.x, this->fx);
    float fy = quadrant_translate_f(camera.y, this->fy);
    float fz = this->fz;

    if (point_fulstrum_test(x,y,z) == false && point_fulstrum_test(fx,fy,fz) == false)
        return;

    Vec3 r = vec3_init(x-fx, y-fy, z-fz);
    normalize_vector(&r);

    Vec3 x1 = vec3_init(x,y,z);
    Vec3 l1 = vec3_sub(x1, camera);

    Vec3 u1 = vec3_cross(l1, r);
    normalize_vector(&u1);

    float ratio = ((float) (HITSCAN_LASER_TTL - ttl))/15.0;
    if(ratio > 1.0) ratio = 1.0;
    float _fx = x + (fx-x)*ratio;
    float _fy = y + (fy-y)*ratio;
    float _fz = z + (fz-z)*ratio;

    Vec3 x2 = vec3_init(_fx, _fy, _fz);

    Vec3 l2 = vec3_sub(x2, camera);

    Vec3 u2 = vec3_cross(l2, r);
    normalize_vector(&u2);

    u1 = vec3_scalar_mult(u1, width);
    u2 = vec3_scalar_mult(u2, width);  

    const float tx_min = 0.0;
    const float tx_max = 1.0;
    const float ty_min = 0.0;
    const float ty_max = 1.0;

    glTexCoord2f(tx_max,ty_max );
    glVertex3f( x2.x + u2.x, x2.y + u2.y, x2.z + u2.z );  // Bottom left
    
    glTexCoord2f(tx_min,ty_max );
    glVertex3f( x1.x + u1.x, x1.y + u1.y, x1.z + u1.z);  // Top left
    
    glTexCoord2f(tx_min,ty_min );
    glVertex3f( x1.x - u1.x, x1.y - u1.y, x1.z - u1.z );  // Top right

    glTexCoord2f(tx_max,ty_min);
    glVertex3f( x2.x - u2.x, x2.y - u2.y, x2.z - u2.z );  // Bottom right

}

void HitscanLaserEffect::draw2(float delta, Vec3 camera)
{
    const float width = 0.5;
    const float height = 12.0;

    float x = quadrant_translate_f(camera.x, this->x);
    float y = quadrant_translate_f(camera.y, this->y);
    float z = this->z;
    
    float fx = quadrant_translate_f(camera.x, this->fx);
    float fy = quadrant_translate_f(camera.y, this->fy);
    float fz = this->fz;

    if (point_fulstrum_test(x,y,z) == false && point_fulstrum_test(fx,fy,fz) == false)
        return;

    Vec3 r = vec3_init(fx-x, fy-y, fz-z);
    normalize_vector( &r );

    float ratio = ((float) (HITSCAN_LASER_TTL - ttl))/30.0;
    if(ratio > 1.0) ratio = 1.0;
    float _fx = x + (fx-x)*ratio;
    float _fy = y + (fy-y)*ratio;
    float _fz = z + (fz-z)*ratio;
    
    Vec3 x1 = vec3_init(_fx, _fy, _fz);
    Vec3 l1 = vec3_sub(x1, camera);

    Vec3 u1 = vec3_cross(l1, r);
    normalize_vector( &u1 );


    Vec3 tmp = vec3_init(r.x*height, r.y*height, r.z*height);
    Vec3 x2 = vec3_sub(x1, tmp);

    Vec3 l2 = vec3_sub(x2, camera);

    Vec3 u2 = vec3_cross( l2, r);
    normalize_vector( &u2 );

    u1 = vec3_scalar_mult(u1, width);
    u2 = vec3_scalar_mult(u2, width);  

    const float tx_min = 0.0;
    const float tx_max = 1.0;
    const float ty_min = 0.0;
    const float ty_max = 1.0;

    glTexCoord2f(tx_max,ty_max );
    glVertex3f( x2.x + u2.x, x2.y + u2.y, x2.z + u2.z );  // Bottom left
    
    glTexCoord2f(tx_min,ty_max );
    glVertex3f( x1.x + u1.x, x1.y + u1.y, x1.z + u1.z);  // Top left
    
    glTexCoord2f(tx_min,ty_min );
    glVertex3f( x1.x - u1.x, x1.y - u1.y, x1.z - u1.z );  // Top right

    glTexCoord2f(tx_max,ty_min);
    glVertex3f( x2.x - u2.x, x2.y - u2.y, x2.z - u2.z );  // Bottom right

}
/* list */

void HitscanLaserEffect_list::draw()
{
    //printf("draw \n");

    int last_tick = _LAST_TICK();
    int _t = _GET_MS_TIME();
    //printf("ms since last update= %i \n", _t - last_tick);
    float delta = ((float)(_t - last_tick)) / 33.3f;
    if (delta > 1.0f) delta = 1.0f;
    delta /= 30.0f;
    //printf("delta= %f \n", delta);

    Vec3 camera = current_camera->get_position();
 
    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    
    GL_ASSERT(GL_DEPTH_TEST, true);

    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);

    glBindTexture( GL_TEXTURE_2D, hitscan_laser_texture_00_id );
    glBegin( GL_QUADS );

    for(int i=0; i<this->num; i++) 
        a[i].draw1(delta, camera); //diffuse beam

    glEnd();

    glBindTexture( GL_TEXTURE_2D, hitscan_laser_texture_01_id );
    glBegin( GL_QUADS );
    for(int i=0; i<this->num; i++) 
        a[i].draw2(delta, camera); //diffuse beam

    glEnd();

    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);

    glDisable(GL_BLEND);

}

void HitscanLaserEffect_list::tick()
{
    //const int debug = 0;

    //if(debug)
    //{
        //static int frame = 0;
        //frame++;

        //if(frame % 8 == 0 )
        //{
            //const float vm = 16.0;
            //float vx = vm*(float)rand()/(float)RAND_MAX;
            //float vy = 160.0; //+vm*(float)rand()/(float)RAND_MAX;
            //float vz = vm*(float)rand()/(float)RAND_MAX;
            //ClientState::hitscan_laser_effect_list->create(32.0, 32.0, 128.0, vx, vy, vz );
        //}

    //}

    //create_hitscan(0.0, 0.0, 0.0, 0.0, 0.0, 160.0);

    //int count= 0;
    for(int i=0; i<this->num; i++)
    {
        a[i].tick();
        a[i].ttl--;
        if(a[i].ttl == 0) destroy(i);
        //count++;
    }
    //printf("count= %i \n", count);
}

}   // Animations

