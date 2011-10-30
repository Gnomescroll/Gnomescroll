#ifndef objects_cspray_h
#define objects_cspray_h

#include <compat.h>
#ifdef DC_CLIENT
#include <compat_gl.h>
#endif

#include <physics/common.h>
#include <ray_trace/ray_trace.h>
#include <t_map/t_map.h>
#include <t_map/t_properties.h>

#ifdef DC_CLIENT
#include <texture_loader.h>
#endif


//struct Cspray {
    //struct Particle2 particle;
    //int active;
    //int stopped;
//};

#define CSPRAY_MAX 4096
#define CSPRAY_TTL 1200
#define CSPRAY_CEMENT_BLOCK_TYPE 2
#define CSPRAY_DAMP 0.0f
#define CSPRAY_TYPE 1


class Cspray {
    public:
        struct Particle2 particle;
        int active;
        int stopped;

        void draw();
        void server_tick();
        void client_tick();

        void _tick();

        Cspray(int _id);
};


#include <c_lib/template/object_list.hpp>

class Cspray_list: public Object_list<Cspray,CSPRAY_MAX>
{
    private:
        const char* name() { return "Cspray"; }
    public:
        void draw();
        void draw(int);
        void server_tick();
        void client_tick();
};


//void init_objects_cspray();
//void cspray_tick();
//void create_cspray(int type, float x, float y, float z, float vx, float vy, float vz);

/*
 *  Client only
 */
#ifdef DC_CLIENT
//void cspray_draw();
#endif

#endif
