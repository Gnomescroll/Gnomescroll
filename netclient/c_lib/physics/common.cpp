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

float distancef(float x, float y, float z, float a, float b, float c)
{
    return sqrt((a-x)*(a-x) + (b-y)*(b-y) + (c-z)*(c-z));
}

float distancef_squared(float x, float y, float z, float a, float b, float c)
{
    return ((a-x)*(a-x) + (b-y)*(b-y) + (c-z)*(c-z));
}

float gaussian_value(const float mean, const float stddev, const float x)
{
    const float sqrt_2pi = sqrt(2*kPI);
    return pow(kE, -((x - mean)*(x - mean))/(2 * stddev * stddev)) / (sqrt_2pi * stddev);
}

void rotate_point(float x, float y, float theta, float *x1, float *y1)
{
    *x1 = x*cos(theta) - y*sin(theta);
    *y1 = x*sin(theta) + y*cos(theta);
}
