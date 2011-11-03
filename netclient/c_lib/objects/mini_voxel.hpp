#pragma once

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


#define MINIVOX_MAX 4096
#define MINIVOX_TTL 30
#define MINIVOX_DAMP 0.5f
#define MINIVOX_TYPE 5
#define MINIVOX_TEXTURE_ID 5
#define MINIVOX_TEXTURE_SCALE 0.1f

#define MINIVOX_DEFAULT_VOXEL {255, 50, 50, 255}

/* also defined in agent/agent_vox.hpp */
struct Voxel {
unsigned char r,g,b,a;
};

class Minivox {
    public:
        Particle2 particle;

        float size;
        Voxel vox;

        void set_color(unsigned char r, unsigned char g, unsigned char b);
        void set_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        void set_size(float s);


        void draw();
        void tick();
        Minivox(int id);
        Minivox(int id, float x, float y, float z, float vx, float vy, float vz);
        ~Minivox();
};

#include <c_lib/template/object_list.hpp>

class Minivox_list: public Object_list<Minivox, MINIVOX_MAX>
{
    private:
        const char* name() { return "Minivox"; }
    public:
        void draw();
        void tick();
};
    
