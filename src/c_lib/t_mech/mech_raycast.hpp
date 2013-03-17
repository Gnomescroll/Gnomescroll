#pragma once

#include <t_mech/_interface.hpp>
#include <t_mech/mech_draw.hpp>

namespace t_mech
{

#if DC_CLIENT
void draw(const struct Mech &m)
{
    #if 0
    float vn[3*8];

    float dx1 = sin(m.rotation * PI);
    float dy1 = cos(m.rotation * PI);

    float dx2 = sin((m.rotation+0.5) * PI);
    float dy2 = cos((m.rotation+0.5) * PI);

    //dx = sin((m.rotation+0.5) * PI);
    //dy = cos((m.rotation+0.5) * PI);

    float _x[4];
    _x[0] = sin(m.rotation * 0.00f*PI);
    _x[1] = sin(m.rotation * 0.50f*PI);
    _x[2] = sin(m.rotation * 1.00f*PI);
    _x[3] = sin(m.rotation * 1.50f*PI);

    float _y[4];
    _y[0] = cos(m.rotation * 0.00f*PI);
    _y[1] = cos(m.rotation * 0.50f*PI);
    _y[2] = cos(m.rotation * 1.00f*PI);
    _y[3] = cos(m.rotation * 1.50f*PI);
    #endif
}

struct Mech* _selected_mech = NULL;


void draw_selected_mech_bounding_box()
{
    if (_selected_mech == NULL)
        return;

    struct Mech m = *_selected_mech;

    const float size = m.size/2.0f;
    //const float size2 = m.size;

    float wx = (float) (m.x) + 0.5f + m.offset_x;
    float wy = (float) (m.y) + 0.5f + m.offset_y;
    float wz = (float) m.z; // + size;

    wx = quadrant_translate_f(current_camera_position.x, wx);
    wy = quadrant_translate_f(current_camera_position.y, wy);

    struct Vec3 f = vec3_init(sin(m.rotation * PI), cos(m.rotation * PI), 0.0f);
    struct Vec3 r = vec3_init(sin((m.rotation+0.5)*PI), cos((m.rotation+0.5)*PI), 0.0f);
    struct Vec3 u = vec3_init(0.0f, 0.0f, 1.0f);

    int tex_id = mech_attributes[m.type].sprite;

    GS_ASSERT(mech_sprite_width[mech_attributes[m.type].sprite] != -1)
    GS_ASSERT(mech_sprite_height[mech_attributes[m.type].sprite] != -1)

    float size_w = size*mech_sprite_width_f[tex_id];
    float size_h = 2.0f*size*mech_sprite_height_f[tex_id];

            //mech_sprite_width_f[i]  = 1.0;
            //mech_sprite_height_f[i] = 1.0;

    visualize_bounding_box(wx,wy,wz + 0.01f,
                           size_w,size_w, size_h,
                           f,r,u);

}

bool ray_cast_mech_render_type_0(const struct Mech &m, float x, float y, float z, float vx, float vy, float vz, float* _distance)
{
/*
    static const int q_set[4*6]=
    {
        4,5,6,7,
        3,2,1,0,
        2,3,7,6,
        0,1,5,4,
        3,0,4,7,
        1,2,6,5
    };
*/
    const float size = m.size/2.0f;
    //const float size2 = m.size;

    float wx = (float) (m.x) + 0.5f + m.offset_x;
    float wy = (float) (m.y) + 0.5f + m.offset_y;
    float wz = (float) m.z + size;

    wx = quadrant_translate_f(current_camera_position.x, wx);
    wy = quadrant_translate_f(current_camera_position.y, wy);

    //float dx = sin(m.rotation * PI);
    //float dy = cos(m.rotation * PI);

    //translate into origin of plane
    //x -= wx;
    //y -= wy;
    //z -= wz;

    //const float size = m.size/2.0f;
    //const float size2 = m.size;

/*
    float a = vx*wx + vy*wy + vz*wz;
    float px = x - a*vx;
    float py = y - a*vy;
    float pz = z - a*vz;

    float distance = px*px + py*py + pz*pz;
*/

/*
bool line_box_test(
    float lx, float ly, float lz,
    float lvx, float lvy, float lvz,
    float bx, float by, float bz,       //center
    float bdx, float bdy, float bdz,    //size
    struct Vec3 f,
    struct Vec3 r,
    struct Vec3 u,

    float* a)
*/
    float a = 0.0f;
    struct Vec3 f = vec3_init(sin(m.rotation * PI), cos(m.rotation * PI), 0.0f);
    struct Vec3 r = vec3_init(sin((m.rotation+0.5)*PI), cos((m.rotation+0.5)*PI), 0.0f);
    struct Vec3 u = vec3_init(0.0f, 0.0f, 1.0f);

    bool ret = line_box_test(x, y, z, vx, vy, vz, wx, wy, wz,
                             size, size, size, f, r, u, &a);

    if (ret)
    {
        printf("intercept: %0.2f %0.2f %0.2f top: %0.2f %0.2f %0.2f \n", x+a*vx, y+a*vy, z+a*vz,  wx+size*u.x, wy+size*u.y, wz+size*u.z);

        //printf("Raycast hit: %f \n", a);
        *_distance = a;
        return true;
    }
    return false;
/*
    float _x
    m.size
    m.rotation
    m.offset_x
    m.offset_y
*/

    //const float size = m.size/2.0f;
    //const float size2 = m.size;

/*
    float dx,dy;

    dx = sin(m.rotation * PI);
    dy = cos(m.rotation * PI);

    vn[3*0+0] = wx - size*dx;
    vn[3*0+1] = wy - size*dy;
    vn[3*0+2] = wz + size2;


    dx = sin((m.rotation+0.5) * PI);
    dy = cos((m.rotation+0.5) * PI);

    vn[3*0+0] = wx - size*dx;
    vn[3*0+1] = wy - size*dy;
    vn[3*0+2] = wz + size2;
*/


/*

*/
/*

    vn[3*0+0] = wx - size*dx;
    vn[3*0+1] = wy - size*dy;
    vn[3*0+2] = wz + size2;

    vn[3*1+0] = wx - size*dx;
    vn[3*1+1] = wy - size*dy;
    vn[3*1+2] = wz;

    vn[3*2+0] = wx + size*dx;
    vn[3*2+1] = wy + size*dy;
    vn[3*2+2] = wz;

    vn[3*3+0] = wx + size*dx;
    vn[3*3+1] = wy + size*dy;
    vn[3*3+2] = wz + size2;

*/
}

bool ray_cast_mech(float x, float y, float z, float vx, float vy, float vz, int* _mech_id, float* _distance)
{
    //int nearest_mech = -1;
    //float distance = 1000.0;

    _selected_mech = NULL;

    *_mech_id = -1;
    *_distance = 0.0f;

    float distance = 1000.0f;
    int mech_id = -1;

    int xi = x;
    int yi = y;
    int zi = z;

    const int cutoff2 = 8*8;

    const int mlm = mech_list->mlm;
    struct Mech* mla = mech_list->mla;

    for (int i=0; i<mlm; i++)
    {
        if (mla[i].id == -1) continue;

        float d;
        bool ret;

        int xd = xi - mla[i].x;
        int yd = yi - mla[i].y;
        int zd = zi - mla[i].z;

        if (xd*xd + yd*yd + zd*zd > cutoff2)
            continue;

        switch (mla[i].render_type)
        {
            case MECH_RENDER_TYPE_0: //MECH_CRYSTAL:
                //do something
                ret = ray_cast_mech_render_type_0(mla[i], x,y,z, vx,vy,vz, &d);
                if (ret)
                {
                    printf("mech raycast hit: %i distance= %f \n", i, d);
                    //return true;
                }

                if (ret && d < distance)
                {
                    distance = d;
                    mech_id = i;
                    GS_ASSERT(i == mla[i].id);
                }

                break;

            case MECH_RENDER_TYPE_1:
            case MECH_RENDER_TYPE_2:
            case MECH_RENDER_TYPE_NONE:
                printf("pack_mech error: unhandled mech type\n");
        }
    }

    if (mech_id != -1)
    {
        *_mech_id = mech_id;
        *_distance = distance;
        _selected_mech = &mla[mech_id];
        return true;
    }
    return false;
}
#endif



}   // t_mech
