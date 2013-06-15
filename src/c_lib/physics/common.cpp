#include "common.hpp"

#include <physics/vec3.hpp>

bool cube_intersects(float x, float y, float z, float w, float h, float d,
                     float x2, float y2, float z2, float w2, float h2, float d2)
{   // do not intersect if (Mx>Px) or (Ox>Nx) or (My>Py) or (Oy>Ny) or (Mz>Pz) or (Oz>Nz).
    if ((x > x2+w2) || (x2 > x+w) || (y > y2+h2) || (y2 > y+h) || (z > z2+d2) || (z2 > z+d))
        return false;
    return true;
}

bool rect_intersects(float x, float y, float w, float h,
                     float x2, float y2, float w2, float h2)
{   // do not intersect if (Mx>Px) or (Ox>Nx) or (My>Py) or (Oy>Ny) or (Mz>Pz) or (Oz>Nz).
    if ((x > x2+w2) || (x2 > x+w) || (y > y2+h2) || (y2 > y+h))
        return false;
    return true;
}

bool bounding_box_intersects(const Vec3& posa, const BoundingBox& boxa,
                             const Vec3& posb, const BoundingBox& boxb)
{
    Vec3 a = posa;
    Vec3 b = posb;
    a.x -= boxa.radius;
    a.y -= boxa.radius;
    b.x -= boxb.radius;
    b.y -= boxb.radius;
    float wa = boxa.radius * 2;
    float wb = boxb.radius * 2;
    float ha = boxa.height;
    float hb = boxb.height;
    return ((a.x < b.x + wb) && (b.x < a.x + wa) &&
            (a.y < b.y + wb) && (b.y < a.y + wa) &&
            (a.z < b.z + hb) && (b.z < a.z + ha));
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

bool position_is_equal(const Vec3& p, float x, float y, float z)
{
    if (p.x != x || p.y != y || p.z != z) return false;
    return true;
}

// 0123 +x,-y,-x,+y
// return integer value of axis orientation where point a is source and point b is dest
int axis_orientation(const Vec3& a, const Vec3& b)
{
    const Vec3 p = vec3_sub(quadrant_translate_position(a, b), a);  // vector to point
    const Vec3 q = vec3_abs(p);

    if (p.x <= 0 && q.x >= q.y) return 0; // +x
    if (p.y >= 0 && q.y >= q.x) return 1; // -y
    if (p.x >= 0 && q.x >= q.y) return 2; // -x
    if (p.y <= 0 && q.y >= q.x) return 3; // +y

    return 0;
}

int side_orientation(const Vec3& a, const Vec3& b)
{   // returns side integer that position a is facing relative to b
    const Vec3 p = vec3_sub(quadrant_translate_position(a, b), a);
    const Vec3 q = vec3_abs(p);

    if (q.z >= q.x && q.z >= q.y)
    {
        if (p.z >= 0)
            return 1;
        else
            return 0;
    }
    if (q.x > q.y)
    {
        if (p.x >= 0)
            return 2;
        else
            return 3;
    }
    else
    {
        if (p.y >= 0)
            return 4;
        else
            return 5;
    }

    return 0;
}

inline bool sphere_line_distance(const Vec3& position, const Vec3& direction,
                                 const Vec3& center, Vec3& intersection, float& rad_sq)
{   // NOTE -- direction MUST be normalized.
    // NOTE -- intersection output variable will NOT be translated automatically
    // Reference: http://www.lighthouse3d.com/tutorials/maths/ray-sphere-intersection/
    // calculate dot product of direction and line from position to sphere
    Vec3 c = vec3_sub(center, position);
    float t = vec3_dot(direction, c);
    // if the dot product is <= 0, its behind us
    if (t <= 0)
    {
        intersection = vec3_init(0);
        rad_sq = map_dim.x * map_dim.x + 10;
        return false;
    }
    else
    {
        float dot = vec3_dot(direction, c);
        intersection = vec3_add(position, vec3_scalar_mult(direction, dot));
        rad_sq = vec3_distance_squared(center, intersection);
        return true;
    }
}

int get_cube_side_from_sides(const Vec3i& side)
{
    if (side.i[0] ==  1) return 2;
    if (side.i[0] == -1) return 3;
    if (side.i[1] ==  1) return 4;
    if (side.i[1] == -1) return 5;
    if (side.i[2] ==  1) return 0;
    if (side.i[2] == -1) return 1;

    GS_ASSERT(false);
    return 0;
}

Vec3i get_sides_from_cube_side(int side_id)
{
    Vec3i side = vec3i_init(0);
    switch (side_id)
    {
        case 2:
            side.x = 1;
            break;
        case 3:
            side.x = -1;
            break;
        case 4:
            side.y = 1;
            break;
        case 5:
            side.y = -1;
            break;
        case 0:
            side.z = 1;
            break;
        case 1:
            side.z = -1;
            break;
        default:
            side.z = 1;
            GS_ASSERT(false);
            return side;
    }
    return side;
}

