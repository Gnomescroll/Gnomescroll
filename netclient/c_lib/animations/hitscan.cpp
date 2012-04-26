#include "hitscan.hpp"

#include <state/client_state.hpp>
#include <stdlib.h>

#include <camera/camera.hpp>

#include <compat_gl.h>
#include <SDL/texture_loader.hpp>

namespace Animations
{

const int hitscan_laser_ttl = 10;

static int hitscan_texture_id;

void init_hitscan()
{
    int i;
    i = create_texture_from_file( (char*) "./media/texture/hitscan/hitscan_01.png", &hitscan_texture_id);
    if (i) { printf("init_hitscan failed with code %d\n", i); }
}

void teardown_hitscan()
{


}

HitscanEffect::HitscanEffect(int id)
:
id(id),
ttl(hitscan_laser_ttl)
{}

HitscanEffect::HitscanEffect()
:
ttl(hitscan_laser_ttl)
{}

// DO NOT USE:: TESTING PURPOSES ONLY
void HitscanEffect::add_plane_bias()
{
    return;
    // biases the laser effect in a circle oriented along the normal plane

    // take cross product of up and look vectors
    // assume vx,vy,vz to be the look vector

    Vec3 look = vec3_init(vx,vy,vz);
    normalize_vector(&look);
    Vec3 up = vec3_init(0,0,1);
    Vec3 right = vec3_cross(look, up);
    normalize_vector(&right);

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
    const float dxy = 0.14;
    const float dz = -0.13;
    this->x += dxy * right.x;
    this->y += dxy * right.y;
    this->z += dz;
}

void HitscanEffect::tick()
{
    const float tick_rate = 30.0f;
    
    x += vx / tick_rate;
    y += vy / tick_rate;
    z += vz / tick_rate;    

    //check for collision with terrain/players
    //play animations for terrain/player collision
}

void HitscanEffect::draw(float delta, Vec3 camera)
{
    const float width = 0.50;
    const float height = 1.0/4.0;   //length per velocity

    Vec3 v = vec3_init(vx,vy,vz);
    normalize_vector(&v);

    Vec3 x1 = vec3_init(x,y,z);
    Vec3 look = vec3_sub(x1, camera);

    Vec3 u1 = vec3_cross(look, v);
    normalize_vector(&u1);
    
    Vec3 x2 = vec3_init(x - height*vx, y - height*vy, z - height*vz);
    look = vec3_sub(x2, camera);

    Vec3 u2 = vec3_cross(look, v);
    normalize_vector(&u2);

    u1 = vec3_scalar_mult(u1, width);
    u2 = vec3_scalar_mult(u2, width);   

    static const float tx_min = 0.0;
    static const float tx_max = 1.0;
    static const float ty_min = 0.0;
    static const float ty_max = 1.0;

    glTexCoord2f(tx_max,ty_max);
    glVertex3f( x2.x + u2.x, x2.y + u2.y, x2.z + u2.z );  // Bottom left
    
    glTexCoord2f(tx_min,ty_max);
    glVertex3f( x1.x + u1.x, x1.y + u1.y, x1.z + u1.z);  // Top left
    
    glTexCoord2f(tx_min,ty_min);
    glVertex3f( x1.x - u1.x, x1.y - u1.y, x1.z - u1.z );  // Top right

    glTexCoord2f(tx_max,ty_min);
    glVertex3f( x2.x - u2.x, x2.y - u2.y, x2.z - u2.z );  // Bottom right

     ////LOOKS BETTER -
    //glTexCoord2f(tx_min,ty_min);
    //glVertex3f( x1.x, x1.y, x1.z );  // Top right

    //glTexCoord2f(tx_max,ty_min);
    //glVertex3f( x2.x, x2.y, x2.z );  // Bottom right
}

/* List */

void HitscanEffect_list::draw()
{
    if (current_camera == NULL)
    {
        printf("HitscanEffect_list::draw() -- current_camera is NULL\n");
        return;
    }

    const float tick_rate = 30.0f;

    int last_tick = _LAST_TICK();
    int _t = _GET_MS_TIME();
    float delta = ((float)(_t - last_tick)) / (100.0f*(1.0f / tick_rate));
    if(delta > 1.0f)
        delta = 1.0f;
    delta /= tick_rate;

    Vec3 camera = vec3_init(current_camera->x, current_camera->y, current_camera->z);

    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    GL_ASSERT(GL_DEPTH_TEST, true);

    glDepthMask(GL_FALSE);

    glBindTexture( GL_TEXTURE_2D, hitscan_texture_id );
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);

    glBegin( GL_QUADS );

    int i;
    for(i=0; i<n_max; i++) 
    {
        if (a[i] == NULL) continue;
        a[i]->draw(delta, camera);
    }
    
    glEnd();
    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

}

void HitscanEffect_list::tick()
{
    //const int debug = 1;
    const int debug = 0;
    HitscanEffect* he;
    if(debug)
    {
        static int frame = 0;
        frame++;

        if(frame % 8 == 0 )
        {
            const float vm = 16.0;
            float vx = vm*(float)rand()/(float)RAND_MAX;
            float vy = 160.0;
            float vz = vm*(float)rand()/(float)RAND_MAX;
            he = hitscan_effect_list->create();
            he->set_state(32.0, 32.0, 64.0, vx, vy, vz);
        }

    }

    int i;
    for(i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        a[i]->tick();
        a[i]->ttl--;
        if(a[i]->ttl <= 0)
            destroy(a[i]->id);
    }
}

}

