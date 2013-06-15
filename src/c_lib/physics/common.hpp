#pragma once

struct Vec3; // forward decl
struct Vec3i; // forward decl

const float kE = 2.71828f;

struct BoundingBox
{
    float radius;
    float height;
};

struct ControlState
{
    int seq;
    float theta;
    float phi;
    uint32_t cs;
};

typedef enum
{
    CS_NULL = 0,
    CS_FORWARD = 1,
    CS_BACKWARD = 2,
    CS_LEFT = 4,
    CS_RIGHT = 8,
    CS_JETPACK = 16,
    CS_JUMP = 32,
    CS_CROUCH = 64,
    CS_BOOST = 128,
    CS_CHARGE = 256,
    CS_MISC2 = 512,
    CS_MISC3 = 1024
} CSKey;

bool cube_intersects(float x, float y, float z, float w, float h, float d,
                     float x2, float y2, float z2, float w2, float h2, float d2);

bool bounding_box_intersects(const Vec3& posa, const BoundingBox& boxa,
                             const Vec3& posb, const BoundingBox& boxb);

bool rect_intersects(float x, float y, float w, float h,
                     float x2, float y2, float w2, float h2);

inline float interpolate(float a, float b, float f)
{
    return a + (b - a) * f;
}

bool point_in_rect(float px, float py, float rx, float ry, float w, float h);

float distancef(float x, float y, float a, float b);
float distancef(float x, float y, float z, float a, float b, float c);
float distancef_squared(float x, float y, float z, float a, float b, float c);
void rotate_point(float x, float y, float theta, float *x1, float *y1);
float gaussian_value(const float mean, const float stddev, const float x);

bool position_is_equal(const Vec3& p, float x, float y, float z);
int axis_orientation(const Vec3& a, const Vec3& b);

inline bool sphere_line_distance(const Vec3& position, const Vec3& direction,
                                 const Vec3& center, Vec3& intersection, float& rad_sq);

int get_cube_side_from_sides(const Vec3i& sides);
Vec3i get_sides_from_cube_side(int side_id);
int side_orientation(const Vec3& a, const Vec3& b);
