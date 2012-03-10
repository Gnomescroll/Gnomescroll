#include "neutron.hpp"

#ifdef DC_CLIENT
#include <compat_gl.h>
#endif

#include <ray_trace/ray_trace.hpp>
#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>

#include <c_lib/state/client_state.hpp>
#include <c_lib/state/server_state.hpp>
#include <c_lib/defines.h>

const float NEUTRON_MASS = 0.1f;

void Neutron::init()
{
    this->ttl_max = NEUTRON_TTL;
    this->type = NEUTRON_TYPE;
    this->texture_index = NEUTRON_TEXTURE_ID;
    this->scale = NEUTRON_TEXTURE_SCALE;
    this->event_ttl = 1;
    energy = rand() % 3 + 1;
    if (energy == 3)
        this->event_ttl = 150;
}

Neutron::Neutron(int id)
:
EventParticle(id, 0,0,0,0,0,0, NEUTRON_MASS)
{
    this->init();
}

Neutron::Neutron(int id, float x, float y, float z, float mx, float my, float mz)
:
EventParticle(id, x,y,z, mx,my,mz, NEUTRON_MASS)
{
    this->init();
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
            newtron = STATE::neutron_list->create(this->vp->p.x,this->vp->p.y,this->vp->p.z, this->vp->v.x,this->vp->v.y,this->vp->v.z);
            newtron->energy = 1;
            newtron = STATE::neutron_list->create(this->vp->p.x,this->vp->p.y,this->vp->p.z, -this->vp->v.x,-this->vp->v.y,-this->vp->v.z);
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

                newtron = STATE::neutron_list->create(this->vp->p.x,this->vp->p.y,this->vp->p.z, this->vp->v.x,this->vp->v.y,this->vp->v.z);
                newtron->energy = 1;
                newtron = STATE::neutron_list->create(this->vp->p.x,this->vp->p.y,this->vp->p.z, -this->vp->v.x,-this->vp->v.y,-this->vp->v.z);
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

void Neutron_list::tick()
{
    for (int i=0; i<n_max; i++)
    {
        if (a[i] == NULL) continue;
        a[i]->tick();
        if (a[i]->ttl >= a[i]->ttl_max)
            destroy(a[i]->id);
    }
}

void Neutron_list::draw() {
#ifdef DC_CLIENT
    if(num == 0) return;

    glColor3ub(255,255,255);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glBindTexture(GL_TEXTURE_2D, particle_texture_id);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glBegin(GL_QUADS);

    for(int i=0; i<n_max; i++)
        if (a[i] != NULL)
            a[i]->draw();

    glEnd();
    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
#endif
}

