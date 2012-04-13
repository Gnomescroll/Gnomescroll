#pragma once

#include <physics/common.hpp>
#include <c_lib/voxel/common.h>

#include <c_lib/particles/minivox_colored.hpp>

namespace Particles
{

class TexturedMinivox: public ParticleMotion
{
    public:
        float theta, phi;
        float dtheta, dphi;
        void orient_vectors();
        void set_spin(float dtheta, float dphi);
        void spin();

        float size;

        Vec3 vec_x;
        Vec3 vec_y;
        Vec3 vec_z;
        
        void set_orientation(float theta, float phi);
        void set_size(float size);
        void set_angles(float theta, float phi);

        float pix_margin;
        float tx,ty;
        
        short texture_pixel_width;
        
        void set_texture(int tex_id);
        void set_texture(int tex_id, int pixels_wide);
        
        void draw();
        void tick();
        
        explicit TexturedMinivox(int id);
        TexturedMinivox(int id, float x, float y, float z, float mx, float my, float mz);
};

}

#include <c_lib/template/object_list.hpp>

namespace Particles
{

class TexturedMinivox_list: public Object_list<TexturedMinivox, MINIVOX_MAX>
{
    private:
        const char* name() { return "TexturedMinivox"; }
    public:
        void draw();
        void tick();

        TexturedMinivox_list() { print(); }
};
    
}
