#include "neutron.hpp"

#pragma once

#include <c_lib/state/client_state.hpp>
#include <c_lib/state/server_state.hpp>
#include <c_lib/defines.h>

#ifdef DC_CLIENT
static float neutron_proj_mtrx[16];
#endif

Neutron::Neutron(int id) {
    create_particle2(&particle, (unsigned int)id, NEUTRON_TYPE, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f, 0, NEUTRON_TTL);
    energy = 1;
    event_ttl = 1;
}

Neutron::Neutron(int id, float x, float y, float z, float vx, float vy, float vz) {
    create_particle2(&particle, (unsigned int)id, NEUTRON_TYPE, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f, 0, NEUTRON_TTL);
    event_ttl = 1;
    energy = rand() % 3 + 1;
    if (energy == 3) {
        event_ttl = 150;
    }
}

/* Can't be accessed via Object_list without adding a create() method with this type sig */
Neutron::Neutron(int id, int _energy, float x, float y, float z, float vx, float vy, float vz) {
    create_particle2(&particle, (unsigned int)id, NEUTRON_TYPE, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f, 0, NEUTRON_TTL);
    event_ttl = 1;
    energy = _energy;
    if (energy == 3) {
        event_ttl = 150;
    }
}

void Neutron::set_energy(int _energy) {
    energy = _energy;
}

void Neutron::tick() {
    int* s;
    int collision[3];
    int tile;
    s = bounce_collide_tile_rk4(&particle, collision, &tile, NEUTRON_DAMP);

    Neutron* newtron;

    if(energy==3) {
        event_ttl--;
        if(event_ttl == 0) {
            newtron = STATE::neutron_list.create(particle.state.p.x,particle.state.p.y,particle.state.p.z, particle.state.v.x,particle.state.v.y,particle.state.v.z);
            newtron->energy = 1;
            newtron = STATE::neutron_list.create(particle.state.p.x,particle.state.p.y,particle.state.p.z, -particle.state.v.x,-particle.state.v.y,-particle.state.v.z);
            newtron->energy = 1;
        }

    }

    if(s[0] != 0 || s[1] != 0 || s[2] != 0)
    {
        if(isNuclear(tile)) {
            particle.ttl=0; //reset TTL
            if(energy ==3) {
                event_ttl = 0;

                float _vx, _vy, _vz, len, vel=2;

                _vx = (float)rand()/(float)RAND_MAX;
                _vy = (float)rand()/(float)RAND_MAX;
                _vz = (float)rand()/(float)RAND_MAX;
                len = sqrt(_vx*_vx+_vy*_vy+_vz*_vz);
                _vx *= vel/len;
                _vy *= vel/len;
                _vz *= vel/len;

                newtron = STATE::neutron_list.create(particle.state.p.x,particle.state.p.y,particle.state.p.z, particle.state.v.x,particle.state.v.y,particle.state.v.z);
                newtron->energy = 1;
                newtron = STATE::neutron_list.create(particle.state.p.x,particle.state.p.y,particle.state.p.z, -particle.state.v.x,-particle.state.v.y,-particle.state.v.z);
                newtron->energy = 1;

            }
            if(energy < 3) {
                energy++;

                if(energy==3) {
                    event_ttl= 60;
                }
            }
        }
    }
    
    particle.ttl++;
}

void Neutron::draw() {

#ifdef DC_CLIENT

    float up[3] = {
        neutron_proj_mtrx[0]*NEUTRON_TEXTURE_SCALE,
        neutron_proj_mtrx[4]*NEUTRON_TEXTURE_SCALE,
        neutron_proj_mtrx[8]*NEUTRON_TEXTURE_SCALE
    };
    float right[3] = {
        neutron_proj_mtrx[1]*NEUTRON_TEXTURE_SCALE,
        neutron_proj_mtrx[5]*NEUTRON_TEXTURE_SCALE,
        neutron_proj_mtrx[9]*NEUTRON_TEXTURE_SCALE
    };

    float tx_min, tx_max, ty_min, ty_max;
    float x,y,z;

    int id = 48+ (3 * particle.type) + (energy-1);

    tx_min = (float)(id%16)* (1.0/16.0);
    tx_max = tx_min + (1.0/16.0);
    ty_min = (float)(id/16)* (1.0/16.0);
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



void Neutron_list::tick() {
    int i;
    for (i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        a[i]->tick();
        if (a[i]->particle.ttl >= a[i]->particle.ttl_max) {
            destroy(a[i]->particle.id);
        }
    }
}

void Neutron_list::draw() {
#ifdef DC_CLIENT

    if(num == 0) { return; }
    glGetFloatv(GL_MODELVIEW_MATRIX, neutron_proj_mtrx);

    glEnable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glBindTexture( GL_TEXTURE_2D, get_particle_texture() );
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

