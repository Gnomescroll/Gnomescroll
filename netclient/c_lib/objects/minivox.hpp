#pragma once

#include <compat.h>
#ifdef DC_CLIENT
    #include <compat_gl.h>
#endif

#include <c_lib/voxel/common.h>
#include <physics/common.h>
#include <ray_trace/ray_trace.h>
#include <t_map/t_map.h>
#include <t_map/t_properties.h>

#ifdef DC_CLIENT
    #include <texture_loader.h>
#endif

#define MINIVOX_MAX 4096
#define MINIVOX_TTL 40
#define MINIVOX_DAMP 0.15f
#define MINIVOX_TYPE 6

/* voxel properties */
#define MINIVOX_SIZE 0.1f
#define MINIVOX_R 184
#define MINIVOX_G 138
#define MINIVOX_B 0
#define MINIVOX_A 150

class Minivox {
    private:
        float theta;
        float phi;
        void orient_vectors();

    public:
        Particle2 particle;

        float size;

        Vector vec_x;
        Vector vec_y;
        Vector vec_z;
        
        Voxel vox;

        void set_color(unsigned char r, unsigned char g, unsigned char b);
        void set_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        void set_orientation(float theta, float phi);

        void draw();
        void tick();
        Minivox(int id);
        Minivox(int id, float x, float y, float z, float vx, float vy, float vz);
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
    
