#include "blood.hpp"
#pragma once

#ifdef DC_CLIENT
static float blood_proj_mtrx[16];
#endif

#include <compat.h>
#ifdef DC_CLIENT
    #include <compat_gl.h>
#endif

#include <ray_trace/ray_trace.h>
#include <t_map/t_map.hpp>
#include <t_map/t_properties.h>

Blood::Blood(int id) {
    create_particle2(&particle, id, BLOOD_TYPE, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f, 0, BLOOD_TTL);
}

Blood::Blood(int id, float x, float y, float z, float vx, float vy, float vz) {
    create_particle2(&particle, id, BLOOD_TYPE, x,y,z,vx,vy,vz, 0, BLOOD_TTL);
}

void Blood::tick() {
    bounce_simple_rk4(&particle, BLOOD_DAMP);
    particle.ttl++;
}

void Blood::draw() {

#ifdef DC_CLIENT

    float up[3] = {
        blood_proj_mtrx[0]*BLOOD_TEXTURE_SCALE,
        blood_proj_mtrx[4]*BLOOD_TEXTURE_SCALE,
        blood_proj_mtrx[8]*BLOOD_TEXTURE_SCALE
    };
    float right[3] = {
        blood_proj_mtrx[1]*BLOOD_TEXTURE_SCALE,
        blood_proj_mtrx[5]*BLOOD_TEXTURE_SCALE,
        blood_proj_mtrx[9]*BLOOD_TEXTURE_SCALE
    };

    float tx_min, tx_max, ty_min, ty_max;
    float x,y,z;

    tx_min = (float)(BLOOD_TEXTURE_ID%16)* (1.0/16.0);
    tx_max = tx_min + (1.0/16.0);
    ty_min = (float)(BLOOD_TEXTURE_ID/16)* (1.0/16.0);
    ty_max = ty_min + (1.0/16.0);

    x=particle.state.p.x; y=particle.state.p.y; z=particle.state.p.z;

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
        if (a[i]->particle.ttl >= a[i]->particle.ttl_max) {
            destroy(a[i]->particle.id);
        }
    }
}

void Blood_list::draw() {
#ifdef DC_CLIENT

    if(num == 0) { return; }
    glGetFloatv(GL_MODELVIEW_MATRIX, blood_proj_mtrx);

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

