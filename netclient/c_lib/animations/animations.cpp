#include "animations.hpp"

// purple color ranges
#define R_MIN 38
#define R_MAX 154
#define G_MIN 6
#define G_MAX 29
#define B_MIN 54
#define B_MAX 256

namespace Animations {

void block_crumble_animation(float x, float y, float z, int n) {

    const float _vx = 10.0f,
                  _vy = 10.0f,
                  _vz = 10.0f;

    int r,g,b;
    float nx,ny,nz;
    float vx,vy,vz;

    Minivox* minivox;
    
    int i;
    for (i=0; i < n; i++) {

        nx = randf() + x;
        ny = randf() + y;
        nz = randf() + z;

        vx = _vx*(randf() -0.5f);
        vy = _vy*(randf() -0.5f);
        vz = _vz*(randf() -0.5f);

        //purple shades
        r = randrange(R_MIN, R_MAX);
        g = randrange(G_MIN, G_MAX);
        b = randrange(B_MIN, B_MAX);
        
        minivox = ClientState::minivox_list.create(nx,ny,nz, vx,vy,vz);
        minivox->set_color(r,g,b);
    }
}

}
