
#include <c_lib/physics/vec3.hpp>

struct Vec4 {
    union
    {
        float f[4];
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
