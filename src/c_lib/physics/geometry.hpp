#pragma once

#include <physics/vec3.hpp>

#if DC_CLIENT
// cached results for visualization
static Vec3 _pl;
static Vec3 _pv;
static Vec3 _pp;
static Vec3 p0;
static Vec3 p1;
static Vec3 _i;
#endif

static bool _line_plane_intersection(const Vec3& position,
                                     const Vec3& direction,
                                     const Vec3& plane_point,
                                     const Vec3& normal,
                                     float& a)
{
    a = 0.0f;
    float num = vec3_dot(vec3_sub(plane_point, position), normal);
    float denum = vec3_dot(direction, normal);

    static const float epsilon = 0.001f;

    float absnum = fabsf(num);
    float absdenum = fabsf(denum);
    if (absnum > epsilon && absdenum > epsilon)
    {   // intersection
        if (denum)
            a = num / denum;
        else
            a = epsilon;
        return true;
    }

    //If the line starts outside the plane and is parallel to the plane, there is no intersection.
    //In this case, the above denominator will be zero and the numerator will be non-zero.

    if (absdenum < epsilon && absnum > epsilon)
    {   // line outside of plane and parallel
        return false;
    }

    //If the line starts inside the plane and is parallel to the plane, the line intersects the plane everywhere.
    //In this case, both the numerator and denominator above will be zero.

    if (absdenum < epsilon && absnum < epsilon)
    {   // intersects everywhere
        a = 0.0f;
        return true;
    }

    GS_ASSERT(false);
    return false;
}

bool line_plane_intersection(const Vec3& position,
                             const Vec3& direction,
                             const Vec3& center,
                             const Vec3& size,
                             const Vec3& n,      //normal
                             const Vec3& v1,     //direction 1 on surface
                             const Vec3& v2,     //direction 2 on surface
                             float& a)           //coefficient
{
    a = 0.0f;
    Vec3 adjpos = vec3_add(center, vec3_scalar_mult(n, size.x));
    if (_line_plane_intersection(position, direction, adjpos, n, a))
    {
        Vec3 p = vec3_add(vec3_sub(position, adjpos),
                          vec3_scalar_mult(direction, a));
        float ta = vec3_dot(p, v1);
        if (ta < size.y && ta > -size.y)
        {
            float tb = vec3_dot(p, v2);
            if (tb < size.z && tb > -size.z)
            {
                #if DC_CLIENT
                // set visualization flags
                _pp = adjpos;
                _pl = position;
                _pv = direction;
                p0 = vec3_scalar_mult(v1, size.y);
                p1 = vec3_scalar_mult(v2, size.z);
                _i = vec3_add(p, adjpos);
                #endif
                return true;
            }
        }
    }
    return false;
}

#if DC_CLIENT
void visualize_line()
{
    glDisable(GL_TEXTURE_2D);
    glColor4ub(255,0,0,255);
    glLineWidth(1.0f);

    const float len = 4.0f;

    glBegin(GL_LINES);

    glVertex3f(_pl.x, _pl.y, _pl.z);
    Vec3 p = vec3_add(_pl, vec3_scalar_mult(_pv, len));
    glVertex3f(p.x, p.y, p.z);

    glEnd();
    glLineWidth(1.0f);

    glColor4ub(0,255,0,255);
    glPointSize(4.0f);
    glBegin(GL_POINTS);

    glVertex3f(_pp.x, _pp.y, _pp.z);

    glEnd();

    glColor4ub(0,0,255,255);
    glPointSize(6.0f);
    glBegin(GL_POINTS);

    p = vec3_add(_pp, p0);
    glVertex3f(p.x, p.y, p.z);
    p = vec3_sub(_pp, p0);
    glVertex3f(p.x, p.y, p.z);
    p = vec3_add(_pp, p1);
    glVertex3f(p.x, p.y, p.z);
    p = vec3_sub(_pp, p1);
    glVertex3f(p.x, p.y, p.z);

    glEnd();

    glColor4ub(0,255,255,255);
    glPointSize(8.0f);
    glBegin(GL_POINTS);

    glVertex3f(_i.x, _i.y, _i.z);

    glEnd();

    glColor4ub(255,255,255,255);
    glPointSize(1.0f);
}

void visualize_bounding_box(const Vec3& center,    //center
                            const Vec3& size,  //size
                            const Vec3& forward,
                            const Vec3& right,
                            const Vec3& up)
{
    static const int vi[2*12] = {
        0,1,
        1,2,
        2,3,
        3,0,

        4,5,
        5,6,
        6,7,
        7,4,

        0,4,
        1,5,
        2,6,
        3,7,
    };

    static const int vs[3*8] = {
        -1,-1,0,
        1,-1,0,
        1,1,0,
        -1,1,0,
        -1,-1,1,
        1,-1,1,
        1,1,1,
        -1,1,1,
    };

    struct Vec3 f = vec3_scalar_mult(forward, size.x);
    struct Vec3 r = vec3_scalar_mult(right, size.y);
    struct Vec3 u = vec3_scalar_mult(up, size.z);

    struct Vec3 vex[8];
    for (int i=0; i<8; i++)
    {
        vex[i] = vec3_add(vec3_add(vec3_add(center,
                                            vec3_scalar_mult(f, vs[3*i+0])),
                                            vec3_scalar_mult(r, vs[3*i+1])),
                                            vec3_scalar_mult(u, vs[3*i+2]));
    }

    glDisable(GL_TEXTURE_2D);
    glColor4ub(0,0,0,255);
    glLineWidth(2.0f);

    glBegin(GL_LINES);

    for (int i=0; i<12; i++)
    {
        int i1 = vi[2*i+0];
        int i2 = vi[2*i+1];
        glVertex3f(vex[i1].x, vex[i1].y, vex[i1].z);
        glVertex3f(vex[i2].x, vex[i2].y, vex[i2].z);
    }

    glEnd();

    glColor4ub(255,255,255,255);
    glLineWidth(1.0f);

}

void visualize_bounding_box()
{
    static const struct Vec3 f = vec3_init(1.0f, 0.0f, 0.0f);
    static const struct Vec3 r = vec3_init(0.0f, 1.0f, 0.0f);
    static const struct Vec3 u = vec3_init(0.0f, 0.0f, 1.0f);
    visualize_bounding_box(_i,               //center
                           vec3_init(1.0f),  //size
                           f, r, u);
}
#endif

bool line_box_test(const Vec3& position,
                   const Vec3& direction,
                   const Vec3& center,       //center
                   const Vec3& size,         //size
                   const Vec3& f,
                   const Vec3& r,
                   const Vec3& u,
                   float& a)
{   // there are 6 planes, do intersection on each of them
    Vec3 s = size;
    if (line_plane_intersection(position, direction, center, s, u, f, r, a))
        return true;
    s.x *= -1;
    if (line_plane_intersection(position, direction, center, s, u, f, r, a))
        return true;

    s = vec3_init(size.y, size.z, size.x);
    if (line_plane_intersection(position, direction, center, s, u, f, r, a))
        return true;
    s.x *= -1;
    if (line_plane_intersection(position, direction, center, s, u, f, r, a))
        return true;

    // top
    s = vec3_init(size.z, size.x, size.y);  // flip the size vector for whatever reason
    if (line_plane_intersection(position, direction, center, s, u, f, r, a))
        return true;
    // bottom
    s.x *= -1;
    if (line_plane_intersection(position, direction, center, s, u, f, r, a))
        return true;

    return false;
}
