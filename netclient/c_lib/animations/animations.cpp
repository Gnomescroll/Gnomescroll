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

void grenade_explode_animation(float x, float y, float z) {
    int n = randrange(8,13);

    float vx = 20.0f;
    float vy = 20.0f;
    float vz = 20.0f;

    float cx,cy,cz;
    float cvx,cvy,cvz;

    int i;
    for (i=0; i<n; i++) {
        cx = x + ((randf() - 0.5f) / 20.0f);
        cy = y + ((randf() - 0.5f) / 20.0f);
        cz = z + ((randf() - 0.5f) / 20.0f);
        cvx = vx * (randf() - 0.5f);
        cvy = vy * (randf() - 0.5f);
        cvz = vz * (randf() - 0.5f);
        ClientState::shrapnel_list.create(cx, cy, cz, cvx, cvy, cvz);
    }
}

}
