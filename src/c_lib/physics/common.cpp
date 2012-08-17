#include "common.hpp"

#include <physics/vec3.hpp>

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

bool rect_intersects(
    float x, float y, float w, float h,
    float x2, float y2, float w2, float h2
)
{   // do not intersect if (Mx>Px) or (Ox>Nx) or (My>Py) or (Oy>Ny) or (Mz>Pz) or (Oz>Nz).
    if
    ((x > x2+w2) || (x2 > x+w) || (y > y2+h2) || (y2 > y+h))
        return false;
    return true;
}

bool point_in_rect(float px, float py, float rx, float ry, float w, float h)
{
    if (px > rx && px < rx+w && py > ry && py < ry+h)
        return true;
    return false;
}

float distancef(float x, float y, float a, float b)
{
    return sqrtf((a-x)*(a-x) + (b-y)*(b-y));
}

float distancef(float x, float y, float z, float a, float b, float c)
{
    return sqrtf((a-x)*(a-x) + (b-y)*(b-y) + (c-z)*(c-z));
}

float distancef_squared(float x, float y, float z, float a, float b, float c)
{
    return (a-x)*(a-x) + (b-y)*(b-y) + (c-z)*(c-z);
}

float gaussian_value(const float mean, const float stddev, const float x)
{
    const float sqrt_2pi = sqrtf(2*kPI);
    return powf(kE, -((x - mean)*(x - mean))/(2 * stddev * stddev)) / (sqrt_2pi * stddev);
}

void rotate_point(float x, float y, float theta, float *x1, float *y1)
{
    *x1 = x*cosf(theta) - y*sinf(theta);
    *y1 = x*sinf(theta) + y*cosf(theta);
}

bool position_is_equal(Vec3 p, float x, float y, float z)
{
    if (p.x != x || p.y != y || p.z != z) return false;
    return true;
}

// 0123 +x,-y,-x,+y
// return integer value of axis orientation where point a is source and point b is dest
int axis_orientation(Vec3 a, Vec3 b)
{
    b = quadrant_translate_position(a,b);
    a = vec3_sub(b,a);  // vector to point

    float ax = abs(a.x);
    float ay = abs(a.y);

    if (a.x <= 0 && ax >= ay) return 0; // +x
    if (a.y >= 0 && ay >= ax) return 1; // -y
    if (a.x >= 0 && ax >= ay) return 2; // -x
    if (a.y <= 0 && ay >= ax) return 3; // +y
    
    return 0;
}
