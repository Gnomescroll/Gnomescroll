#include "blood.hpp"

#ifdef DC_CLIENT
#include <compat_gl.h>
#include <c_lib/camera/camera.hpp>
#endif

#include <ray_trace/ray_trace.h>
#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>

const float BLOOD_MASS = 0.1f;

Blood::Blood(int id)
:
Particle(id, 0,0,0,0,0,0, BLOOD_MASS)
{
    this->ttl_max = BLOOD_TTL;
    this->type = BLOOD_TYPE;
}

Blood::Blood(int id, float x, float y, float z, float mx, float my, float mz)
:
Particle(id, x,y,z, mx,my,mz, BLOOD_MASS)
{
    this->ttl_max = BLOOD_TTL;
    this->type = BLOOD_TYPE;
}

void Blood::tick() {
    Verlet::bounce(this->vp, BLOOD_DAMP);
    this->ttl++;
}

void Blood::draw() {

#ifdef DC_CLIENT
    if (current_camera == NULL || !current_camera->in_view(this->vp->p.x, this->vp->p.y, this->vp->p.z)) return;


    float up[3] = {
        model_view_matrix[0]*BLOOD_TEXTURE_SCALE,
        model_view_matrix[4]*BLOOD_TEXTURE_SCALE,
        model_view_matrix[8]*BLOOD_TEXTURE_SCALE
    };
    float right[3] = {
        model_view_matrix[1]*BLOOD_TEXTURE_SCALE,
        model_view_matrix[5]*BLOOD_TEXTURE_SCALE,
        model_view_matrix[9]*BLOOD_TEXTURE_SCALE
    };

    float tx_min, tx_max, ty_min, ty_max;
    float x,y,z;

    tx_min = (float)(BLOOD_TEXTURE_ID%16)* (1.0/16.0);
    tx_max = tx_min + (1.0/16.0);
    ty_min = (float)(BLOOD_TEXTURE_ID/16)* (1.0/16.0);
    ty_max = ty_min + (1.0/16.0);

    x=this->vp->p.x; y=this->vp->p.y; z=this->vp->p.z;

    glTexCoord2f(tx_min,ty_max );
    glVertex3f(x+(-right[0]-up[0]), y+(-right[1]-up[1]), z+(-right[2]-up[2]));  // Bottom left

    glTexCoord2f(tx_min,ty_min );
    glVertex3f(x+(up[0]-right[0]), y+(up[1]-right[1]), z+(up[2]-right[2]));  // Top left

    glTexCoord2f(tx_max,ty_min);
    glVertex3f(x+(up[0]+right[0]), y+(up[1]+right[1]), z+(up[2]+right[2]));  // Top right

    glTexCoord2f(tx_max,ty_max );
    glVertex3f(x+(right[0]-up[0]), y+(right[1]-up[1]), z+(right[2]-up[2]));  // Bottom right

#endif    
}


/* Blood list */

void Blood_list::tick() {
    int i;
    for (i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        a[i]->tick();
        if (a[i]->ttl >= a[i]->ttl_max) {
            destroy(a[i]->id);
        }
    }
}

void Blood_list::draw() {
#ifdef DC_CLIENT

    if(num == 0) { return; }

    glColor3ub(255,255,255);
    glEnable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glBindTexture( GL_TEXTURE_2D, particle_texture_id );
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);

    glBegin( GL_QUADS );

    int i;
    for(i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        a[i]->draw();
    }

    glEnd();
    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);
    glDisable (GL_DEPTH_TEST);
    glDisable(GL_BLEND);

#endif
}

