#include "common.hpp"

bool cube_intersects(
    float x, float y, float z, float w, float h, float d,
    float x2, float y2, float z2, float w2, float h2, float d2
)
{   // do not intersect if (Mx>Px) or (Ox>Nx) or (My>Py) or (Oy>Ny) or (Mz>Pz) or (Oz>Nz).
    if
    ((x > x2+w2) || (x2 > x+w) || (y > y2+h2) || (y2 > y+h) || (z > z2+d2) || (z2 > z+d))
        return false;
    return true;
}
