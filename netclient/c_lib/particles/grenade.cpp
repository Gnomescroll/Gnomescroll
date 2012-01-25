#include "grenade.hpp"
#pragma once

#ifdef DC_CLIENT
#include <c_lib/animations/animations.hpp>
static float grenade_proj_mtrx[16];
#endif

#ifdef DC_CLIENT
    #include <compat_gl.h>
#endif

#include <ray_trace/ray_trace.h>
#include <t_map/t_map.hpp>
#include <t_map/t_properties.h>

#include <c_lib/state/client_state.hpp>
#include <c_lib/state/server_state.hpp>

#include <common/enum_types.hpp>

/*
 *  Networking; spawn packet from server to client
 */

#include <net_lib/net.hpp>

class grenade_StoC: public FixedSizeNetPacketToClient<grenade_StoC>
{
    public:

        float x,y,z;
        float vx,vy,vz;
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
            pack_u16(&ttl_max, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
        }

        inline void handle();

        grenade_StoC(Grenade* g);
        grenade_StoC() {
            //its faster to not set values
            //only set default values, not required values in intializer
        /*
            x=0.0f;
            y=0.0f;
            z=0.0f;
            vx=0.0f;
            vy=0.0f;
            vz=0.0f;
        */
            //this->id = id; //wtf does this mean, id=id?
            ttl_max = GRENADE_TTL;
            type = GRENADE_TYPE;

        }
};


Grenade::Grenade(int id) : owner(-1) {
    create_particle2(&particle, id, GRENADE_TYPE, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f, 0, GRENADE_TTL);
                   // particle, _id,      type,      x,y,z,         vx,vy,vz,   ttl,  ttl_max
    #ifdef DC_SERVER
    grenade_StoC g = grenade_StoC(this);
    g.broadcast();
    #endif
}

Grenade::Grenade(int id, float x, float y, float z, float vx, float vy, float vz) : owner(-1) {
    create_particle2(&particle, id, GRENADE_TYPE, x,y,z,vx,vy,vz, 0, GRENADE_TTL);
                   // particle, _id,      type,   x,y,z vx,vy,vz, ttl, ttl_max
    #ifdef DC_SERVER
    grenade_StoC g = grenade_StoC(this);
    g.broadcast();
    #endif
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

void Grenade::explode() {
#ifdef DC_CLIENT
    Animations::grenade_explode(particle.state.p.x, particle.state.p.y, particle.state.p.z);
#endif

#ifdef DC_SERVER
    int i;
    // find all agents in radius, apply damage
    ServerState::agent_list.agents_within_sphere(particle.state.p.x, particle.state.p.y, particle.state.p.z, GRENADE_AGENT_DAMAGE_RADIUS);

    Agent_state* a;
    for (i=0; i<ServerState::agent_list.n_filtered; i++) {
        a = ServerState::agent_list.filtered_agents[i];
        if (a == NULL) continue;
        a->status.apply_damage(GRENADE_SPLASH_DAMAGE, owner, OBJ_TYPE_AGENT); // need to be able to pass owner & suicidal arguments to apply_damage
    }
    
    // find all blocks in radius, destroy/damage
    damage_blocks();
#endif
}

int Grenade::block_damage(int dist) {
    int max_dist = GRENADE_BLOCK_DESTROY_RADIUS*3;
    float ratio = (float)(max_dist - dist) / (float)(max_dist);
    float dmg = ratio * (float)(GRENADE_BLOCK_DAMAGE);
    int idmg = (int)(dmg);
    return idmg;
}

void Grenade::damage_blocks() {
#ifdef DC_SERVER
    float x = particle.state.p.x;
    float y = particle.state.p.y;
    float z = particle.state.p.z;

    int ir = GRENADE_BLOCK_DESTROY_RADIUS;

    int ix = (int)x;
    int iy = (int)y;
    int iz = (int)z;
    
    int i,j,k;
    int bx,by,bz;
    int res;
    int dmg;
    for (i=0; i<ir; i++) {
        for (j=0; j<ir; j++) {
            for (k=0; k<ir; k++) {
                dmg = block_damage(i+j+k);

                bx = ix + i;
                by = iy + j;
                bz = iz + k;
                res = _apply_damage(bx,by,bz, dmg);
                if (res==0) { _block_broadcast(bx,by,bz,0); }
                bx = ix - i;
                res = _apply_damage(bx,by,bz, dmg);
                if (res==0) { _block_broadcast(bx,by,bz,0); }
                by = iy - j;
                res = _apply_damage(bx,by,bz, dmg);
                if (res==0) { _block_broadcast(bx,by,bz,0); }
                by = iy + j;
                bz = iz - k;
                res = _apply_damage(bx,by,bz, dmg);
                if (res==0) { _block_broadcast(bx,by,bz,0); }
                bx = ix + i;
                by = iy - j;
                res = _apply_damage(bx,by,bz, dmg);
                if (res==0) { _block_broadcast(bx,by,bz,0); }
                bx = ix - i;
                res = _apply_damage(bx,by,bz, dmg);
                if (res==0) { _block_broadcast(bx,by,bz,0); }
            }
        }
    }
#endif
}

/* Grenade list */
void Grenade_list::tick() {
    int i;
    for(i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        a[i]->tick();

        if(a[i]->particle.ttl >= a[i]->particle.ttl_max) {
            a[i]->explode();
            destroy(a[i]->particle.id);
            num--;
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
    #ifdef DC_CLIENT
    //printf("Spawn grenade particle: %i \n", id);
    Grenade* g = ClientState::grenade_list.create((int)id, x, y, z, vx, vy, vz);
    g->particle.ttl_max = (int)ttl_max;
    g->particle.type = (int)type;
    #endif
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
