#pragma once

#include <compat.h>
#ifdef DC_CLIENT
    #include <compat_gl.h>
#endif

#include <physics/common.h>
#include <ray_trace/ray_trace.h>
#include <t_map/t_map.hpp>
#include <t_map/t_properties.h>
#include <c_lib/template/object_list.hpp>
#include <c_lib/template/net.hpp>

/* These should be pulled from a dat or something */
#define GRENADE_MAX 1024
#define GRENADE_TTL 100
#define GRENADE_DAMP 0.3f
#define GRENADE_TYPE 1
#define GRENADE_TEXTURE_ID 5
#define GRENADE_TEXTURE_SCALE 1.0f

#define GRENADE_BLOCK_DESTROY_RADIUS 2
#define GRENADE_AGENT_DAMAGE_RADIUS 2.0f
#define GRENADE_SPLASH_DAMAGE 20
#define GRENADE_BLOCK_DAMAGE 32

class Grenade {
    public:
        struct Particle2 particle;

        void draw();
        void tick();
        void set_ttl(int ttl);

        void explode();

        int block_damage(int dist);
        void damage_blocks();

        
        Grenade(int id);
        Grenade(int id, float x, float y, float z, float vx, float vy, float vz);
};

class Grenade_list: public Object_list<Grenade, GRENADE_MAX>
{
    private:
        const char* name() { return "Grenade"; }
    public:
        void draw();
        void tick();
};

/*
 *  Networking; spawn packet from server to client
 */

class grenade_StoC: public FixedSizeNetPacketToClient<grenade_StoC>
{
    public:

        float x,y,z;
        float vx,vy,vz;
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
