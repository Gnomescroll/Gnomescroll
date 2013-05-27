#include "mech_raycast.hpp"

#if DC_CLIENT
# include <t_mech/mech_draw.hpp>
#endif

#include <physics/geometry.hpp>

namespace t_mech
{

static struct Mech* _selected_mech = NULL;

#if DC_CLIENT
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
    Vec3 w = get_mech_center(m);
    w = quadrant_translate_position(current_camera_position, w);
    w.z += 0.01f;

    struct Vec3 f = vec3_init(sinf(m.rotation * PI), cosf(m.rotation * PI), 0.0f);
    struct Vec3 r = vec3_init(sinf((m.rotation+0.5)*PI), cosf((m.rotation+0.5)*PI), 0.0f);
    struct Vec3 u = vec3_init(0.0f, 0.0f, 1.0f);
    Vec3 size = t_mech::get_mech_box_dimensions(m);
    visualize_bounding_box(w, size, f, r, u);
}
#endif

static bool ray_cast_mech_render_type_0(const struct Mech &m, const Vec3& position,
                                        const Vec3& direction, float& distance)
{
    Vec3 w = get_mech_center(m);
    w = quadrant_translate_position(position, w);
    w.z += 0.01f;   // lift it off the ground a little, so it doesn't
                    // conflict with terrain
    float d = 1000000000.0f;
    distance = d;
    struct Vec3 f = vec3_init(sinf(m.rotation * PI), cosf(m.rotation * PI), 0.0f);
    struct Vec3 r = vec3_init(sinf((m.rotation+0.5)*PI), cosf((m.rotation+0.5)*PI), 0.0f);
    struct Vec3 u = vec3_init(0.0f, 0.0f, 1.0f);

    Vec3 size = get_mech_box_dimensions(m);
    size.z *= 0.5f;
    w.z += size.z;
    bool ret = line_box_test(position, direction, w, size,
                             f, r, u, d);
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
        Vec3 center = get_mech_center(mla[i]);
        center = quadrant_translate_position(position, center);
        float rad_sq = get_mech_radius(mla[i]);
        rad_sq *= rad_sq;
        if (vec3_length_squared(vec3_sub(center, position)) > range_squared - rad_sq)
            continue;

        float d = 1000000.0f;
        bool ret = false;
        switch (mla[i].render_type)
        {
            case MECH_RENDER_TYPE_0: //MECH_CRYSTAL:
            case MECH_RENDER_TYPE_1:
            case MECH_RENDER_TYPE_2:
            case MECH_RENDER_TYPE_3:
            case MECH_RENDER_TYPE_4:
                ret = ray_cast_mech_render_type_0(mla[i], position, direction, d);
                if (ret && d < nearest_distance)
                {
                    nearest_distance = d;
                    nearest_mech_id = i;
                    GS_ASSERT(i == mla[i].id);
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

}   // t_mech
