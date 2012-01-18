#include <c_lib/physics/vec3.hpp>
#include <c_lib/physics/vec4.hpp>

struct Vec4 {
    union
    {
        float f[16];
        struct 
        {
            float x,y,z,w;
        };
        struct 
        {
            Vec3 v3;    
        };
    };
};
