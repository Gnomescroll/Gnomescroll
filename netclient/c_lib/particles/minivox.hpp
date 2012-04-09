#pragma once

#include <physics/common.hpp>
#include <c_lib/voxel/common.h>

namespace Particles
{

#define MINIVOX_MAX 4096
#define MINIVOX_TTL 100
#define MINIVOX_DAMP 0.5f
#define MINIVOX_TYPE 6

/* voxel properties */
float minivox_size = 0.05f;
#define MINIVOX_R 136
#define MINIVOX_G 27
#define MINIVOX_B 224
#define MINIVOX_A 255

#define MINIVOX_DRAW_MODE_TEXTURED 1

class Minivox: public Particle
{
    public:
        float theta, phi;
        float dtheta, dphi;
        void orient_vectors();
        void set_spin(float dtheta, float dphi);
        void spin();

        unsigned char r,g,b,a;

        float size;

        Vec3 vec_x;
        Vec3 vec_y;
        Vec3 vec_z;
        
        void set_color(unsigned char r, unsigned char g, unsigned char b);
        void set_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        void set_orientation(float theta, float phi);
        void set_size(float size);
        void set_angles(float theta, float phi);

        float pix_margin;
        float tx,ty;
        
        short draw_mode;
        short texture_pixel_width;
        
        void set_texture(int tex_id);
        void set_texture(int tex_id, int pixels_wide);
        
        void draw_colored();
        void draw_textured();
        void tick();
        
        explicit Minivox(int id);
        Minivox(int id, float x, float y, float z, float mx, float my, float mz);
};

#include <c_lib/template/object_list.hpp>

class Minivox_list: public Object_list<Minivox, MINIVOX_MAX>
{
    private:
        const char* name() { return "Minivox"; }
    public:
        void draw_colored();
        void draw_textured();
        void tick();

        float _s;
        void set_size(float s);
        void unset_size();

        Minivox_list();
};
    
}