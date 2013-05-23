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
    if (_selected_mech == NULL)
        return;

    struct Mech m = *_selected_mech;

    const float size = m.size/2.0f;
    //const float size2 = m.size;

    Vec3 w = vec3_add(vec3_init(m.position),
                      vec3_init(0.5f + m.offset_x, 0.5f + m.offset_x, 0.0f));
    w = quadrant_translate_position(current_camera_position, w);

    struct Vec3 f = vec3_init(sinf(m.rotation * PI), cosf(m.rotation * PI), 0.0f);
    struct Vec3 r = vec3_init(sinf((m.rotation+0.5)*PI), cosf((m.rotation+0.5)*PI), 0.0f);
    struct Vec3 u = vec3_init(0.0f, 0.0f, 1.0f);

    int tex_id = mech_attributes[m.type].sprite;

    GS_ASSERT(mech_sprite_width[mech_attributes[m.type].sprite] != -1)
    GS_ASSERT(mech_sprite_height[mech_attributes[m.type].sprite] != -1)

    float size_w = size*mech_sprite_width_f[tex_id];
    float size_h = 2.0f*size*mech_sprite_height_f[tex_id];

    //mech_sprite_width_f[i]  = 1.0;
    //mech_sprite_height_f[i] = 1.0;

    w.z += 0.01;
    visualize_bounding_box(w, vec3_init(size_w, size_w, size_h), f, r, u);
}
#endif

static bool ray_cast_mech_render_type_0(const struct Mech &m, const Vec3& position, const Vec3& direction, float& distance)
{
    const float size = m.size * 0.5f;
    Vec3 w = vec3_add(vec3_init(m.position), vec3_init(0.5f, 0.5f, size));
    w = vec3_add(w, vec3_init(m.offset_x, m.offset_y, 0.0f));
    w = quadrant_translate_position(position, w);
    float d = 0.0f;
    struct Vec3 f = vec3_init(sin(m.rotation * PI), cos(m.rotation * PI), 0.0f);
    struct Vec3 r = vec3_init(sin((m.rotation+0.5)*PI), cos((m.rotation+0.5)*PI), 0.0f);
    struct Vec3 u = vec3_init(0.0f, 0.0f, 1.0f);

    bool ret = line_box_test(position, direction, w, vec3_init(size), f, r, u, d);
    if (ret)
        distance = d;
    return ret;
}

bool ray_cast_mech(const Vec3& position, const Vec3& direction, int& mech_id, float& distance)
{
    //int nearest_mech = -1;
    //float distance = 1000.0;

    _selected_mech = NULL;

    mech_id = NULL_MECH_ID;
    distance = 1000.0f;

    const Vec3i ipos = vec3i_init(position);

    const int cutoff_squared = 8*8;
    const int mlm = mech_list->mlm;
    struct Mech* mla = mech_list->mla;

    float nearest_distance = 10000.0f;
    int nearest_mech_id = NULL_MECH_ID;

    for (int i=0; i<mlm; i++)
    {
        if (mla[i].id == NULL_MECH_ID) continue;
        Vec3i p = vec3i_sub(ipos, mla[i].position);
        if (vec3i_length_squared(p) > cutoff_squared)
            continue;

        float d = 0.0f;
        bool ret = false;
        switch (mla[i].render_type)
        {
            case MECH_RENDER_TYPE_0: //MECH_CRYSTAL:
                //do something
                ret = ray_cast_mech_render_type_0(mla[i], position, direction, d);
                if (ret)
                {
                    printf("mech raycast hit: %d, distance: %0.3f\n", i, d);
                    //return true;
                }
                if (ret && d < nearest_distance)
                {
                    nearest_distance = d;
                    nearest_mech_id = i;
                    GS_ASSERT(i == mla[i].id);
                }
                break;

            case MECH_RENDER_TYPE_1:
            case MECH_RENDER_TYPE_2:
            case MECH_RENDER_TYPE_3:
            case MECH_RENDER_TYPE_4:
                printf("%s warning: mech_type %d not handled\n", __FUNCTION__,
                       mla[i].render_type);
                break;

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
        return true;
    }
    return (nearest_mech_id != NULL_MECH_ID);
}

}   // t_mech
