#include "neutron.hpp"

#ifdef DC_CLIENT
#include <compat_gl.h>
#endif

#include <ray_trace/ray_trace.h>
#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>

#include <c_lib/state/client_state.hpp>
#include <c_lib/state/server_state.hpp>
#include <c_lib/defines.h>

Neutron::Neutron(int id)
:
EventParticle(id, 0,0,0,0,0,0, DEFAULT_MASS)
{
    this->ttl_max = NEUTRON_TTL;
    this->type = NEUTRON_TYPE;
    this->event_ttl = 1;
    energy = rand() % 3 + 1;
    if (energy == 3)
        this->event_ttl = 150;
}

Neutron::Neutron(int id, float x, float y, float z, float vx, float vy, float vz)
:
EventParticle(id, x,y,z, vx,vy,vz, DEFAULT_MASS)
{
    this->ttl_max = NEUTRON_TTL;
    this->type = NEUTRON_TYPE;
    this->event_ttl = 1;
    energy = rand() % 3 + 1;
    if (energy == 3)
        this->event_ttl = 150;
}

void Neutron::set_energy(int energy)
{
    this->energy = energy;
}

void Neutron::tick()
{
    int* s;
    int collision[3];
    int tile;
    s = Verlet::bounce(this->vp, collision, &tile, NEUTRON_DAMP);

    Neutron* newtron;

    if(energy==3)
    {
        event_ttl--;
        if(event_ttl == 0)
        {
            newtron = STATE::neutron_list.create(this->vp->p.x,this->vp->p.y,this->vp->p.z, this->vp->v.x,this->vp->v.y,this->vp->v.z);
            newtron->energy = 1;
            newtron = STATE::neutron_list.create(this->vp->p.x,this->vp->p.y,this->vp->p.z, -this->vp->v.x,-this->vp->v.y,-this->vp->v.z);
            newtron->energy = 1;
        }

    }

    if(s[0] != 0 || s[1] != 0 || s[2] != 0)
    {
        if(isNuclear(tile))
        {
            ttl=0; //reset TTL
            if(energy ==3)
            {
                event_ttl = 0;

                float _vx, _vy, _vz, len, vel=2;

                _vx = (float)rand()/(float)RAND_MAX;
                _vy = (float)rand()/(float)RAND_MAX;
                _vz = (float)rand()/(float)RAND_MAX;
                len = sqrt(_vx*_vx+_vy*_vy+_vz*_vz);
                _vx *= vel/len;
                _vy *= vel/len;
                _vz *= vel/len;

                newtron = STATE::neutron_list.create(this->vp->p.x,this->vp->p.y,this->vp->p.z, this->vp->v.x,this->vp->v.y,this->vp->v.z);
                newtron->energy = 1;
                newtron = STATE::neutron_list.create(this->vp->p.x,this->vp->p.y,this->vp->p.z, -this->vp->v.x,-this->vp->v.y,-this->vp->v.z);
                newtron->energy = 1;

            }
            if(energy < 3)
            {
                energy++;
                if(energy==3)
                    event_ttl= 60;
            }
        }
    }
    
    ttl++;
}

void Neutron::draw() {

#ifdef DC_CLIENT

    float up[3] = {
        model_view_matrix[0]*NEUTRON_TEXTURE_SCALE,
        model_view_matrix[4]*NEUTRON_TEXTURE_SCALE,
        model_view_matrix[8]*NEUTRON_TEXTURE_SCALE
    };
    float right[3] = {
        model_view_matrix[1]*NEUTRON_TEXTURE_SCALE,
        model_view_matrix[5]*NEUTRON_TEXTURE_SCALE,
        model_view_matrix[9]*NEUTRON_TEXTURE_SCALE
    };

    float tx_min, tx_max, ty_min, ty_max;
    float x,y,z;

    int id = 48+ (3 * type) + (energy-1);

    tx_min = (float)(id%16)* (1.0/16.0);
    tx_max = tx_min + (1.0/16.0);
    ty_min = (float)(id/16)* (1.0/16.0);
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



void Neutron_list::tick() {
    int i;
    for (i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        a[i]->tick();
        if (a[i]->ttl >= a[i]->ttl_max) {
            destroy(a[i]->id);
        }
    }
}

void Neutron_list::draw() {
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

