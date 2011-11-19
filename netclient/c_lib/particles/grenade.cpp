#include "grenade.hpp"
#pragma once

#ifdef DC_CLIENT
static float grenade_proj_mtrx[16];
#endif

Grenade::Grenade(int id) {
    create_particle2(&particle, id, GRENADE_TYPE, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f, 0, GRENADE_TTL);
                   // particle, _id,      type,      x,y,z,         vx,vy,vz,   ttl,  ttl_max
    #ifdef DC_SERVER
    grenade_StoC g = grenade_StoC(this);
    g.broadcast();
    #endif
}

Grenade::Grenade(int id, float x, float y, float z, float vx, float vy, float vz) {
    create_particle2(&particle, id, GRENADE_TYPE, x,y,z,vx,vy,vz, 0, GRENADE_TTL);
                   // particle, _id,      type,   x,y,z vx,vy,vz, ttl, ttl_max
    #ifdef DC_SERVER
    grenade_StoC g = grenade_StoC(this);
    g.broadcast();
    #endif
}

Grenade::Grenade(grenade_StoC* g) {
    create_particle2(&particle, g->id, g->type, g->x, g->y, g->z, g->vx, g->vy, g->vz, 0, g->ttl_max);
}

void Grenade::tick() {
    bounce_simple_rk4(&particle, GRENADE_DAMP);
    particle.ttl++;
}

void Grenade::set_ttl(int ttl) {
    particle.ttl = ttl;
}

void Grenade::draw() {
#ifdef DC_CLIENT

    float up[3] = {
        grenade_proj_mtrx[0]*GRENADE_TEXTURE_SCALE,
        grenade_proj_mtrx[4]*GRENADE_TEXTURE_SCALE,
        grenade_proj_mtrx[8]*GRENADE_TEXTURE_SCALE
    };
    float right[3] = {
        grenade_proj_mtrx[1]*GRENADE_TEXTURE_SCALE,
        grenade_proj_mtrx[5]*GRENADE_TEXTURE_SCALE,
        grenade_proj_mtrx[9]*GRENADE_TEXTURE_SCALE
    };


    float tx_min, tx_max, ty_min, ty_max;
    float x,y,z;

    tx_min = (float)(GRENADE_TEXTURE_ID%16)* (1.0/16.0);
    tx_max = tx_min + (1.0/16.0);
    ty_min = (float)(GRENADE_TEXTURE_ID/16)* (1.0/16.0);
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

/* Grenade list */

void Grenade_list::tick() {
    int i;
    for(i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        a[i]->tick();

        // destruction currently handled by python,
        // because python is tracking grenades in a Projectile List
        // and needs to play animation (needs to retrieve grenade
        // position before it gets destroyed)
        //if(a[i]->particle.ttl >= a[i]->particle.ttl_max) {
            //destroy(a[i]->particle.id);
        //}
    }
}

void Grenade_list::draw() {
#ifdef DC_CLIENT

    if(num == 0) { return; }
    glGetFloatv(GL_MODELVIEW_MATRIX, grenade_proj_mtrx);

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

inline void print_grenade(Grenade *g) {
    printf("Print Grenade -\n");
    printf("ID: %d\n", g->particle.id);
    printf("Pos: %0.2f %0.2f %0.2f\n", g->particle.state.p.x, g->particle.state.p.y, g->particle.state.p.z);
    printf("Vel: %0.2f %0.2f %0.2f\n", g->particle.state.v.x, g->particle.state.v.y, g->particle.state.v.z);
    printf("TTL max: %d\n", g->particle.ttl_max);
    printf("Type: %d\n", g->particle.type);
}
inline void grenade_StoC::handle() {
    printf("Spawn grenade particle: %i \n", id);
    Grenade* g = new Grenade(this);
    print_grenade(g);
}

grenade_StoC::grenade_StoC(Grenade *g) {

    x = g->particle.state.p.x;
    y = g->particle.state.p.y;
    z = g->particle.state.p.z;
    vx = g->particle.state.v.x;
    vy = g->particle.state.v.y;
    vz = g->particle.state.v.z;
    ttl_max = g->particle.ttl_max - g->particle.ttl;
    id = g->particle.id;
    type = g->particle.type;
}
