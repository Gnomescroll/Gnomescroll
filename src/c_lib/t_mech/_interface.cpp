#include "_interface.hpp"

#include <t_mech/net/CtoS.hpp>
#include <t_mech/net/StoC.hpp>
#include <t_mech/mech_state.hpp>

#if DC_CLIENT
# include <camera/camera.hpp>
# include <physics/ray_trace/hitscan.hpp>
# include <t_mech/mech_draw.hpp>
# include <physics/geometry.hpp>
#endif

namespace t_mech
{

struct MechList* mech_list;

#if DC_CLIENT
class MechListRenderer* mech_list_renderer = NULL;
#endif

void init_packets()
{
    mech_create_StoC::register_client_packet();
    mech_type_change_StoC::register_client_packet();
    mech_delete_StoC::register_client_packet();
}

void init()
{
    GS_ASSERT(mech_list == NULL);
    mech_list = new MechList;
    #if DC_CLIENT
    GS_ASSERT(mech_list_renderer == NULL);
    mech_list_renderer = new MechListRenderer;
    #endif
}

void teardown()
{
    delete mech_list;
    #if DC_CLIENT
    delete mech_list_renderer;
    #endif
}

#if DC_CLIENT
void prep()
{
    mech_list_renderer->prep_vbo();
}

void draw()
{
    mech_list_renderer->draw();
}

void draw_transparent()
{
    mech_list_renderer->draw_transparent();
}
#endif


#if DC_SERVER

//pack mech data into packet

static void pack_mech(struct Mech &m, class mech_create_StoC &p)
{
    p.id = m.id;
    p.type = m.type;
    p.subtype = m.subtype;
    p.x = m.x;
    p.y = m.y;
    p.z = m.z;

    GS_ASSERT(mech_attributes[m.type].type != -1);

    switch (mech_attributes[m.type].class_type)
    {
        case MECH_CRYSTAL:
            break;
        case MECH_CROP:
            break;
        case MECH_MYCELIUM:
            break;
        case MECH_WIRE:
        case MECH_SWITCH:
        case NULL_MECH_CLASS:
            printf("pack_mech error: unhandled mech type\n");
    }
}
#endif


//handles unpacking
#if DC_CLIENT

//call after type or subtype changes
static bool _mech_update(struct Mech &m)
{
    class MechAttribute* ma = get_mech_attribute(m.type);

    switch (ma->class_type)
    {
        case MECH_CRYSTAL:
            //do something
            m.render_type = ma->render_type;

            m.size = 0.80f;  //diameter
            m.rotation = 0.25*(rand()%4) + 0.25f*randf()/3;

            m.rotation = 0.0f;
            m.offset = rand()%255;
            //m.subtype = rand()%6;

            m.offset_x = (randf()-0.5f)* (1.0f-m.size);
            m.offset_y = (randf()-0.5f)* (1.0f-m.size);

            m.offset_x = 0.0f;
            m.offset_y = 0.0f;
            m.size = 1.00;
            break;
        case MECH_CROP:
            break;
        case MECH_MYCELIUM:
            break;

        case MECH_WIRE:
        case MECH_SWITCH:
        case NULL_MECH_CLASS:
            GS_ASSERT(false);
            return false;
    }

    return true;
}

static bool unpack_mech(struct Mech &m, class mech_create_StoC &p)
{
    IF_ASSERT(!isValid((MechType)p.type)) return false;

    m.id = p.id;
    m.type = (MechType)p.type;
    m.subtype = p.subtype;
    m.x = p.x;
    m.y = p.y;
    m.z = p.z;

    return _mech_update(m);
}

//ray cast and draw outlines
void client_ray_cast()
{
    //struct Vec3 pos  = agent_camera->get_position();
    //struct Vec3 look = agent_camera->forward_vector();
}
#endif


#if DC_SERVER

//called 6 times per second
void tick()
{
    const int mlm = mech_list->mlm;
    struct Mech* mla = mech_list->mla;

    for (int i=0; i<mlm; i++)
    {
        if (mla[i].id == -1) continue;

        MechType type = mla[i].type;
        MechBehaviorType behavior_type = mech_attributes[type].behavior_type;

        int light_value = 0;
        switch (behavior_type)
        {
            case MECH_BEHAVIOR_TYPE_PLANT:
                if (rand() % 6 != 0)
                    continue;
                GS_ASSERT(mla[i].growth_ttl > 0);
                mla[i].growth_ttl--;
                if (mla[i].growth_ttl == 0)
                    force_mech_growth(i);
            case MECH_BEHAVIOR_TYPE_LIGHT_PLANT:
                break;

            case MECH_BEHAVIOR_TYPE_DARK_PLANT:
                light_value = t_map::get_skylight(mla[i].x, mla[i].y, mla[i].z);
                if (light_value < 10)
                {
                    if (rand() % 6 != 0)
                        continue;
                    GS_ASSERT(mla[i].growth_ttl > 0);
                    mla[i].growth_ttl--;
                    if (mla[i].growth_ttl == 0)
                        force_mech_growth(i);
                }
                break;

            case MECH_BEHAVIOR_TYPE_DEFAULT:
                break;
            default:
                GS_ASSERT(false);
                break;
        }

    }
}

void floating_removal_tick() //removes floating t_mech
{
    const int mlm = mech_list->mlm;
    struct Mech* mla = mech_list->mla;

    int collection_count = 0;
    for (int i=0; i<mlm; i++)
    {
        if (mla[i].id == -1) continue;

        int x = mla[i].x;
        int y = mla[i].y;
        int z = mla[i].z;
        if (!t_map::isSolid(x,y,z-1))
        {
            remove_mech(i);
            collection_count++;
        }
    }

    if (collection_count != 0)
        printf("t_mech::floating_removal_tick, removed %i floating t_mech \n", collection_count);
}

void force_mech_growth(int mech_id)
{
    const int mlm = mech_list->mlm;
    struct Mech* mla = mech_list->mla;

    MechType type = mla[mech_id].type;
    //MechBehaviorType behavior_type = mech_attributes[type].behavior_type;

    MechType growth_stage = mech_attributes[type].growth_stage;  //next growth stage

    GS_ASSERT(isValid(growth_stage));
    GS_ASSERT(mla[mech_id].id != -1);
    GS_ASSERT(mech_id >= 0 && mech_id < mlm);

    mla[mech_id].type =  growth_stage;
    mla[mech_id].growth_ttl = mech_attributes[growth_stage].growth_ttl;

    class mech_type_change_StoC p;
    p.id = mech_id;
    p.type = growth_stage;
    p.broadcast();
}

bool create_mech(int x, int y, int z, MechType type, int subtype)
{
    IF_ASSERT(!isValid(type))
    {
        printf("Mech type %d invalid\n", type);
        return false;
    }

    // TODO -- check valid mech type properly
    IF_ASSERT(!mech_attributes[type].loaded)
    {
        printf("Mech type %d not in use\n", type);
        return false;
    }

    if (!can_place_mech(x,y,z, 0))
    {
        if (t_map::isSolid(x,y,z))
        {
            printf("Can't place mech: point is solid (%d,%d,%d)\n", x,y,z);
            return false;
        }

        if (!t_map::isSolid(x,y,z-1))
        {
            printf("Can't place mech: no solid block below (%d,%d,%d)\n", x,y,z);
            return false;
        }

        if (mech_list->is_occupied(x,y,z))
        {
            printf("Can't place mech: mech already here (%d,%d,%d)\n", x,y,z);
            return false;
        }

        printf("Can't place mech: invalid position (%d,%d,%d)\n", x,y,z);
        return false;
    }

    struct Mech m;
    m.type = type;
    m.subtype = subtype;
    m.x = x;
    m.y = y;
    m.z = z;
    m.growth_ttl = mech_attributes[type].growth_ttl;
    mech_list->server_add_mech(m);

    return true;
}

bool create_mech(int x, int y, int z, MechType type)
{
    return create_mech(x,y,z,type,0);
}

bool create_crystal(int x, int y, int z, MechType type)
{
    MechClassType mech_class = get_mech_class(type);
    IF_ASSERT(mech_class != MECH_CRYSTAL) return false;
    return create_mech(x,y,z, type);
}
#endif

bool can_place_mech(int x, int y, int z, int side)
{
    if (z <= 0 || z > 128) return false;
    if (side != 0) return false;

    if (t_map::isSolid(x,y,z)) return false;
    if (!t_map::isSolid(x,y,z-1)) return false;

    if (mech_list->is_occupied(x,y,z)) return false;

    return true;
}

void place_vine(int x, int y, int z, int side)
{


}


void tick(int x, int y, int z)
{
    #ifdef DC_CLIENT
    //printf("create crystal: %d %d %d \n", x,y,z);
    //create_crystal(x,y,z);
    #endif
}

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

#if DC_SERVER

void send_client_mech_list(ClientID client_id)
{
    mech_list->send_mech_list_to_client(client_id);
}

void handle_block_removal(int x, int y, int z)
{
    MechType type = mech_list->handle_block_removal(x,y,z);
    if (type == NULL_MECH_TYPE) return;
    IF_ASSERT(!isValid(type)) return;

    // drop item from mech
    if (mech_attributes[type].item_drop)
        handle_drop(x,y,z, type);
}

bool remove_mech(int mech_id)   //removes mech with drop
{
    GS_ASSERT(mech_id >= 0 && mech_id < mech_list->mlm);

    struct Mech m = mech_list->mla[mech_id];
    MechType type = m.type;
    IF_ASSERT(!isValid(type)) return false;

    bool ret = mech_list->server_remove_mech(mech_id);
    GS_ASSERT(ret);

    if (mech_attributes[type].item_drop)
        handle_drop(m.x,m.y,m.z, type);

    return ret;
}

int count_mech(MechType type)
{
    return mech_list->count(type);
}


#endif

}   // t_mech
