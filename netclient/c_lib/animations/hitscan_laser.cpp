#include "hitscan_laser.hpp"

#include <c_lib/state/client_state.hpp>
#include <stdlib.h>

#include <c_lib/camera/camera.hpp>

#include <compat_gl.h>
#include <c_lib/SDL/texture_loader.h>
#include <c_lib/physics/vector.hpp>

namespace Animations
{

static int hitscan_laser_texture_00_id;
static int hitscan_laser_texture_01_id;

void init_hitscan_laser()
{
    int i;
    i = create_texture_from_file( (char*) "./media/texture/hitscan/laser_beam_00.png", &hitscan_laser_texture_00_id);
    if (i) { printf("init_hitscan_laser failed with code %d\n", i); }
    i = create_texture_from_file( (char*) "./media/texture/hitscan/laser_beam_core_00.png", &hitscan_laser_texture_01_id);
    if (i) { printf("init_hitscan_laser failed with code %d\n", i); }
}


HitscanLaserEffect::HitscanLaserEffect(int id)
:
id(id),
ttl(hitscan_lader_ttl)
{}

HitscanLaserEffect::HitscanLaserEffect(float x, float y, float z, float fx, float fy, float fz)
:
x(x), y(y), z(z),
fx(fx), fy(fy), fz(fz),
ttl(hitscan_lader_ttl)
{}

HitscanLaserEffect::HitscanLaserEffect(int id, float x, float y, float z, float fx, float fy, float fz)
:
id(id),
x(x), y(y), z(z),
fx(fx), fy(fy), fz(fz),
ttl(hitscan_lader_ttl)
{}



void HitscanLaserEffect::tick()
{
    //check for collision with terrain/players
    //play animations for terrain/player collision
}

void HitscanLaserEffect::draw1(float delta, Vector* camera)
{
    const float width = 0.5;

    struct Vector r = Vector_init(f-fx, y-fy, z-fz);

    normalize_vector( &r );
    
    struct Vector x1 = Vector_init(x,y,z);
    struct Vector l1 = sub_vec(&x1, camera);

    struct Vector u1 = vector_cross( l1, r);
    normalize_vector( &u1 );

    float ratio = ((float) (hitscan_lader_ttl - ttl))/15.0;
    if(ratio > 1.0) ratio = 1.0;
    float _fx = x + (fx-x)*ratio;
    float _fy = y + (fy-y)*ratio;
    float _fz = z + (fz-z)*ratio;

    //float _fx = fx - (fx-x)*ratio;
    //float _fy = fy - (fy-y)*ratio;
    //float _fz = fz - (fz-z)*ratio;

    struct Vector x2 = Vector_init(_fx, _fy, _fz);

    struct Vector l2 = sub_vec(&x2, camera);

    struct Vector u2 = vector_cross( l2, r);
    normalize_vector( &u2 );

    vector_scalar1(&u1, width);
    vector_scalar1(&u2, width);  

/*
    Vector top_left = Vector_init(x1.x + width*u1.x, x1.y + width*u1.y, x1.z + width*u1.z);
    Vector top_right = Vector_init(x1.x - width*u1.x, x1.y - width*u1.y, x1.z - width*u1.z);
    Vector bottom_right = Vector_init(x2.x - width*u2.x, x2.y - width*u2.y, x2.z - width*u2.z);
    Vector bottom_left = Vector_init(x2.x + width*u2.x, x2.y + width*u2.y, x2.z + width*u2.z);
*/
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

void HitscanLaserEffect::draw2(float delta, Vector* camera)
{
    const float width = 0.7;

    //struct Vector r = Vector_init(x-fx, y-fy, z-fz);
    struct Vector r = Vector_init(fx-x, fy-y, fz-z);
    normalize_vector( &r );


    float ratio = ((float) (hitscan_lader_ttl - ttl))/30.0;
    if(ratio > 1.0) ratio = 1.0;
    float _fx = x + (fx-x)*ratio;
    float _fy = y + (fy-y)*ratio;
    float _fz = z + (fz-z)*ratio;
    
    
        
    struct Vector x1 = Vector_init(_fx, _fy, _fz);
    struct Vector l1 = sub_vec(&x1, camera);

    struct Vector u1 = vector_cross( l1, r);
    normalize_vector( &u1 );


    //float _fx = fx - (fx-x)*ratio;
    //float _fy = fy - (fy-y)*ratio;
    //float _fz = fz - (fz-z)*ratio;

    struct Vector x2 = Vector_init(x,y,z);

    struct Vector l2 = sub_vec(&x2, camera);

    struct Vector u2 = vector_cross( l2, r);
    normalize_vector( &u2 );

    vector_scalar1(&u1, width);
    vector_scalar1(&u2, width);  

/*
    Vector top_left = Vector_init(x1.x + width*u1.x, x1.y + width*u1.y, x1.z + width*u1.z);
    Vector top_right = Vector_init(x1.x - width*u1.x, x1.y - width*u1.y, x1.z - width*u1.z);
    Vector bottom_right = Vector_init(x2.x - width*u2.x, x2.y - width*u2.y, x2.z - width*u2.z);
    Vector bottom_left = Vector_init(x2.x + width*u2.x, x2.y + width*u2.y, x2.z + width*u2.z);
*/
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
    if(delta > 1.0f)
    {
        delta = 1.0f;
    }
    delta /= 30.0f;
    //printf("delta= %f \n", delta);

    struct Vector camera = Vector_init(current_camera->x, current_camera->y, current_camera->z);
 
    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);

    glBindTexture( GL_TEXTURE_2D, hitscan_laser_texture_00_id );
    glBegin( GL_QUADS );

    for(int i=0; i<n_max; i++) 
    {
        if (a[i] == NULL) continue;
        a[i]->draw1(delta, &camera); //diffuse beam
    }
    glEnd();

    glBindTexture( GL_TEXTURE_2D, hitscan_laser_texture_01_id );
    glBegin( GL_QUADS );
    for(int i=0; i<n_max; i++) 
    {
        if (a[i] == NULL) continue;
        a[i]->draw2(delta, &camera); //diffuse beam
    }
    glEnd();

    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);
    glDisable (GL_DEPTH_TEST);
    glDisable(GL_BLEND);

}

void HitscanLaserEffect_list::tick()
{
    //printf("tick \n");

    const int debug = 1;

    if(debug)
    {
        static int frame = 0;
        frame++;

        if(frame % 8 == 0 )
        {
        const float vm = 16.0;
        float vx = vm*(float)rand()/(float)RAND_MAX;
        float vy = 160.0; //+vm*(float)rand()/(float)RAND_MAX;
        float vz = vm*(float)rand()/(float)RAND_MAX;
        ClientState::hitscan_laser_effect_list.create(32.0, 32.0, 128.0, vx, vy, vz );
        }

    }

    //create_hitscan(0.0, 0.0, 0.0, 0.0, 0.0, 160.0);

    //int count= 0;
    int i;
    for(i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        a[i]->tick();
        a[i]->ttl--;
        if(a[i]->ttl == 0) destroy(a[i]->id);
        //count++;
    }
    //printf("count= %i \n", count);
}

}

