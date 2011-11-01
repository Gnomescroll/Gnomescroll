#include "grenade.hpp"

float grenade_proj_mtrx[16];

Grenade::Grenade(int id) {
    create_particle2(&particle, id, GRENADE_TYPE, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f, 0, GRENADE_TTL);
                   // particle, _id,      type,      x,y,z,         vx,vy,vz,   ttl,  ttl_max
}

Grenade::Grenade(int id, float x, float y, float z, float vx, float vy, float vz) {
    create_particle2(&particle, id, GRENADE_TYPE, x,y,z,vx,vy,vz, 0, GRENADE_TTL);
                   // particle, _id,      type,   x,y,z vx,vy,vz, ttl, ttl_max
}

void Grenade::tick() {
    bounce_simple_rk4(&particle, GRENADE_DAMP);
    particle.ttl++;
}

void Grenade::draw() {
#ifdef DC_CLIENT

    float up[3] = {grenade_proj_mtrx[0], grenade_proj_mtrx[4], grenade_proj_mtrx[8]};
    float right[3] = {grenade_proj_mtrx[1], grenade_proj_mtrx[5], grenade_proj_mtrx[9]};

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
        if(a[i]->particle.ttl >= a[i]->particle.ttl_max) {
            destroy(a[i]->particle.id);
        }
    }
}

void Grenade_list::draw() {
#ifdef DC_CLIENT

    if(num == 0) { return; }
    glGetFloatv(GL_MODELVIEW_MATRIX, grenade_proj_mtrx);

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


/*
 *  Networking; spawn packet from server to client
 */


#include <c_lib/template/net.hpp>

class grenade_StoC: public FixedSizeNetPacketToClient<grenade_StoC>
{
    public:

        float x,y,z;
        float vx,vy,vz;
        uint16_t ttl;
        uint16_t ttl_max;
        uint16_t id;
        uint8_t type;

        inline void packet(unsigned char* buff, int* buff_n, bool pack) 
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
            printf("Spawn grenade particle: %i \n", id);
        }
};
