#include "cspray.hpp"

#ifdef DC_CLIENT
#include <compat_gl.h>
#endif

#include <ray_trace/ray_trace.h>
#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>

Cspray::Cspray(int id)
:
EventParticle(id, 0,0,0,0,0,0),
active(0),
stopped(0)
{
    this->ttl_max = CSPRAY_TTL;
    this->type = CSPRAY_TYPE;
    this->event_ttl = 1;
}

Cspray::Cspray(int id, float x, float y, float z, float vx, float vy, float vz)
:
EventParticle(id, x,y,z,vx,vy,vz),
active(0),
stopped(0)
{
    this->ttl_max = CSPRAY_TTL;
    this->type = CSPRAY_TYPE;
    this->event_ttl = 1;
}

void Cspray::tick() {

    ttl++;

    int* s;
    int collision[3];
    int tile;

    s = Verlet::bounce(this->vp, collision, &tile, CSPRAY_DAMP);

    // cement effect
    if(active == 1) {
        ttl= ttl_max;
        if(!isActive(tile)) {
            _set(collision[0],collision[1],collision[2], CSPRAY_CEMENT_BLOCK_TYPE);    // create block
            ttl = ttl_max;
            return;
        }
    }

    if(s[0] != 0 || s[1] != 0 || s[2] != 0)
    {
        if(isActive(tile)) {
            active=1;
            ttl *= 2;

            _set(collision[0] - s[0], collision[1] - s[1], collision[2] - s[2], CSPRAY_CEMENT_BLOCK_TYPE);    // create block
            ttl = ttl_max;
            return;

        }
    }

    /* TEST: Stop cspray falling indefinitely */
    if (this->vp->p.z < -1.0f) {
        this->vp->p.z = 0.0f;
        this->vp->v.x = 0.0f;
        this->vp->v.y = 0.0f;
        this->vp->v.z = 0.0f;

        ttl = ttl_max;
    }
}



void Cspray::draw() {
#ifdef DC_CLIENT

    float size = 0.3;
    float up[3] = {model_view_matrix[0]*size, model_view_matrix[4]*size, model_view_matrix[8]*size};
    float right[3] = {model_view_matrix[1]*size, model_view_matrix[5]*size, model_view_matrix[9]*size};

    float tx_min, tx_max, ty_min, ty_max;

    float x,y,z;

    tx_min = (float)(CSPRAY_TEXTURE_ID%16)* (1.0/16.0);
    tx_max = tx_min + (1.0/16.0);
    ty_min = (float)(CSPRAY_TEXTURE_ID/16)* (1.0/16.0);
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


/* Cspray_list */

void Cspray_list::tick() {

    int i;
    for (i=0; i<n_max; i++) {

        if (a[i] == NULL) continue;
        a[i]->tick();
        if (a[i]->ttl >= a[i]->ttl_max) {
            destroy(a[i]->id);
        }

    }
}

void Cspray_list::draw() {
#ifdef DC_CLIENT
    if (num==0) return;

    glColor3ub(255,255,255);
    glEnable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glBindTexture( GL_TEXTURE_2D, particle_texture_id );
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);

    glBegin( GL_QUADS );

    int i;
    for (i=0; i<n_max; i++) {
        if (a[i] != NULL) {
            a[i]->draw();
        }
    }
    
    glEnd();

    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);
    glDisable (GL_DEPTH_TEST);
    glDisable(GL_BLEND);

#endif
}


/*
 *  Networking; spawn packet from server to client
 */


#include <net_lib/net.hpp>

class cspray_StoC: public FixedSizeNetPacketToClient<cspray_StoC>
{
    public:

        float x,y,z;
        float vx,vy,vz;
        uint16_t ttl;
        uint16_t ttl_max;
        uint16_t id;
        uint8_t type;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);

            pack_float(&vx, buff, buff_n, pack);
            pack_float(&vy, buff, buff_n, pack);
            pack_float(&vz, buff, buff_n, pack);

            pack_u16(&id, buff, buff_n, pack);
            pack_u16(&ttl, buff, buff_n, pack);
            pack_u16(&ttl_max, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
        }

        inline void handle() {
            printf("Spawn cspray particle: %i \n", id);
        }
};

