#pragma once

#include <physics/common.hpp>
#include <voxel/common.hpp>

namespace Particle
{

const int MINIVOX_MAX = 4096;
const int MINIVOX_TTL = 100;
const float MINIVOX_DAMP = 0.5f;
const int MINIVOX_TYPE = 6;

/* voxel properties */
const float MINIVOX_MASS = 2.0f;
const float MINIVOX_SIZE = 0.05f;
const unsigned char MINIVOX_R = 136;
const unsigned char MINIVOX_G = 27;
const unsigned char MINIVOX_B = 224;
const unsigned char MINIVOX_A = 255;


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

        inline void reset();
        ColoredMinivox();
};

}

#include <common/template/object_list.hpp>

namespace Particle
{

class ColoredMinivox_list: public Simple_object_list<ColoredMinivox, MINIVOX_MAX>
{
    private:
        const char* name() { return "ColoredMinivox"; }
    public:
        void draw();
        void tick();

        ColoredMinivox_list() { print_list((char*)this->name(), this); }
};
    
}
