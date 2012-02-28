#include "verlet.hpp"

namespace Verlet
{



    
    //int* move(Vec3* v, float damp){}

    bool bounce(VerletParticle* v, float damp)
    {
        // save current state
        // calculate next state
        // check collision over interval
        
        // if interval < 1: bounce
        // recompute delta with fractional interval (introduces minor inaccuracies)
        // reflect
        // dampen

        bool bounced = false;
        
        v->compute();

        float interval = 0.0f;
        int *s = _ray_cast4(
            v->old_p.x, v->old_p.y, v->old_p.z,
            v->p.x, v->p.y, v->p.z,
            &interval
        );

        if (interval < 1.0f)
        {   // collision
            bounced = true;
            v->recompute(interval);
            Vec3 norm = vec3_init(s[0], s[1], s[2]);
            v->v = vec3_reflect(v->v, norm);
            v->v = vec3_scalar_mult(v->v, damp);
        }
        
        return bounced;
    }

    int* bounce(VerletParticle* v, int* collision, int* tile, float damp)
    {   // same as simple bounce, but gets extra metadata on the bounce
        v->compute();

        float interval = 0.0f;
        int *s = _ray_cast5_capped(
            v->old_p.x, v->old_p.y, v->old_p.z,
            v->p.x, v->p.y, v->p.z,
            &interval, collision, tile
        );

        if (interval < 1.0f)
        {   // collision
            v->recompute(interval);
            Vec3 norm = vec3_init(s[0], s[1], s[2]);
            v->v = vec3_reflect(v->v, norm);
            v->v = vec3_scalar_mult(v->v, damp);
        }
        
        return s;
    }

}
