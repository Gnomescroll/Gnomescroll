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
const float MINIVOX_MASS = 2.0f;
const float MINIVOX_SIZE = 0.05f;
#define MINIVOX_R 136
#define MINIVOX_G 27
#define MINIVOX_B 224
#define MINIVOX_A 255


class ColoredMinivox: public ParticleMotion
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

        void draw();
        void tick();
        
        explicit ColoredMinivox(int id);
        ColoredMinivox(int id, float x, float y, float z, float mx, float my, float mz);
};

}

#include <c_lib/template/object_list.hpp>

namespace Particles
{

class ColoredMinivox_list: public Object_list<ColoredMinivox, MINIVOX_MAX>
{
    private:
        const char* name() { return "ColoredMinivox"; }
    public:
        void draw();
        void tick();

        ColoredMinivox_list() { print(); }
};
    
}
