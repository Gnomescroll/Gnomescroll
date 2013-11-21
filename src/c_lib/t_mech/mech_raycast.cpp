/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "mech_raycast.hpp"

#if DC_CLIENT
# include <t_mech/mech_draw.hpp>
#endif

#include <physics/geometry.hpp>

namespace t_mech
{

#if DC_CLIENT

static struct Mech* _selected_mech = NULL;

void draw(const Mech &m)
{
    #if 0
    //float vn[3*8];

    //float dx1 = sin(m.rotation * PI);
    //float dy1 = cos(m.rotation * PI);

    //float dx2 = sin((m.rotation+0.5) * PI);
    //float dy2 = cos((m.rotation+0.5) * PI);

    float _x[4];
    for (int i=0; i<4; i++)
        _x[i] = cosf(m.rotation * 0.5f * PI * i);

    float _y[4];
    for (int i=0; i<4; i++)
        _y[i] = cosf(m.rotation * 0.5f * PI * i);
    #endif
}

void draw_selected_mech_bounding_box()
{
    if (_selected_mech == NULL) return;
    struct Mech m = *_selected_mech;
    Vec3 w = m.center;
    w = quadrant_translate_position(current_camera_position, w);
    w.z += 0.01f;

    struct Vec3 f = vec3_init(sinf(m.rotation * PI), cosf(m.rotation * PI), 0.0f);
    struct Vec3 r = vec3_init(sinf((m.rotation+0.5)*PI), cosf((m.rotation+0.5)*PI), 0.0f);
    struct Vec3 u = vec3_init(0.0f, 0.0f, 1.0f);
    Vec3 size = t_mech::get_mech_box_dimensions(m.type);
    visualize_bounding_box(w, size, f, r, u);
}

static bool ray_cast_mech_render_type_0(const struct Mech &m, const Vec3& center,
                                        const Vec3& position, const Vec3& direction,
                                        float& distance)
{   // center is a repeated argument, its should be pre-translated
    Vec3 w = m.center;
    w.z += 0.01f;   // lift it off the ground a little, so it doesn't
                    // conflict with terrain
    float d = 1000000000.0f;
    distance = d;
    struct Vec3 f = vec3_init(sinf(m.rotation * PI), cosf(m.rotation * PI), 0.0f);
    struct Vec3 r = vec3_init(sinf((m.rotation+0.5)*PI), cosf((m.rotation+0.5)*PI), 0.0f);
    struct Vec3 u = vec3_init(0.0f, 0.0f, 1.0f);

    Vec3 size = get_mech_box_dimensions(m.type);
    size.z *= 0.5f;
    w.z += size.z;
    bool ret = line_box_test(position, direction, w, size, f, r, u, d);
    if (ret)
        distance = d;
    return ret;
}

bool ray_cast_mech(const Vec3& position, const Vec3& direction, float range,
                   int& mech_id, float& distance)
{
    _selected_mech = NULL;
    mech_id = NULL_MECH_ID;
    distance = 1000.0f;

    const float range_squared = range * range;
    const int mlm = mech_list->mlm;
    struct Mech* mla = mech_list->mla;

    float nearest_distance = 10000.0f;
    int nearest_mech_id = NULL_MECH_ID;

    for (int i=0; i<mlm; i++)
    {
        if (mla[i].id == NULL_MECH_ID) continue;
        // TODO -- radius should be precalculated on init, for the mech type
        // center should be computed and stored on create/update for the mech
        // would save ~1ms or so per frame
        Vec3 center = quadrant_translate_position(position, mla[i].center);
        //printf("Position: "); vec3i_print(mla[i].position);
        //printf("Center: "); vec3_print(mla[i].center);
        float rad = get_mech_radius(mla[i].type);
        //printf("Radius: %0.2f\n", rad);
        if (vec3_distance_squared(center, position) > range_squared)
            continue;
        Vec3 collision;
        float sphere_rad_sq;
        if (!sphere_line_distance(position, direction, center, collision, sphere_rad_sq))
            continue;

        // NOTE -- there is a bug
        // we check against rad * 2 because sphere_rad_sq is too large.
        // the sphere_line_distance may be bugged, or
        // the mech center may be wrong, or the radius is wrong
        // (although they are correct for the line box test, so they may only be misrepresented)
        if (sphere_rad_sq > rad * 2)
            continue;

        float d = 1000000.0f;
        switch (mla[i].render_type)
        {
            case MECH_RENDER_TYPE_0: //MECH_CRYSTAL:
            case MECH_RENDER_TYPE_1:
            case MECH_RENDER_TYPE_2:
            case MECH_RENDER_TYPE_3:
            case MECH_RENDER_TYPE_4:
                if (ray_cast_mech_render_type_0(mla[i], center, position,
                                                direction, d) &&
                    d < nearest_distance)
                {
                    nearest_distance = d;
                    nearest_mech_id = i;
                }
                break;

            //case MECH_RENDER_TYPE_1:
            //case MECH_RENDER_TYPE_2:
            //case MECH_RENDER_TYPE_3:
            //case MECH_RENDER_TYPE_4:
                //printf("%s warning: mech_type %d not handled\n", __FUNCTION__,
                       //mla[i].render_type);
                //break;

            case MECH_RENDER_TYPE_NONE:
                GS_ASSERT(false);
                break;
        }
    }

    if (nearest_mech_id != NULL_MECH_ID)
    {
        mech_id = nearest_mech_id;
        distance = nearest_distance;
        _selected_mech = &mla[mech_id];
    }
    return (nearest_mech_id != NULL_MECH_ID);
}
#endif

}   // t_mech
