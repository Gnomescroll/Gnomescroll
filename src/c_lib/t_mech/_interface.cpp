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
    p.side = m.side;
    p.x = m.x;
    p.y = m.y;
    p.z = m.z;

    GS_ASSERT(mech_attributes[m.type].type != -1);
    GS_ASSERT((m.side >=0 && m.side <= 5) || m.side == 255);

    switch (mech_attributes[m.type].class_type)
    {
        case MECH_CRYSTAL:
            break;
        case MECH_CROP:
            break;
        case MECH_MYCELIUM:
            break;
        case MECH_SIGN:
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

            //m.rotation = 0.0f;
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
        case MECH_SIGN:
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
    m.side = p.side;
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


        MechType type = mla[i].type;
        MechBehaviorType behavior_type = mech_attributes[type].behavior_type;

        int x = mla[i].x;
        int y = mla[i].y;
        int z = mla[i].z;

        switch (behavior_type)
        {
            case MECH_BEHAVIOR_TYPE_PLANT:
            case MECH_BEHAVIOR_TYPE_LIGHT_PLANT:
            case MECH_BEHAVIOR_TYPE_DARK_PLANT:
                if (!t_map::isSolid(x,y,z-1))
                {
                    remove_mech(i);
                    collection_count++;
                }
                break;
            case MECH_BEHAVIOR_TYPE_DEFAULT:
                break;
            default:
                GS_ASSERT(false);
                break;
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

    int ret = can_place_mech(x,y,z, type, 0);
    if (ret != 0)
    {
        if (t_map::isSolid(x,y,z))
        {
            printf("Can't place mech: point is solid (%d,%d,%d), ret_code= %d \n", x,y,z,ret);
            return false;
        }

        if (!t_map::isSolid(x,y,z-1))
        {
            printf("Can't place mech: no solid block below (%d,%d,%d), ret_code= %d \n", x,y,z,ret);
            return false;
        }

        if (mech_list->is_occupied(x,y,z))
        {
            printf("Can't place mech: mech already here (%d,%d,%d), ret_code= %d \n", x,y,z,ret);
            return false;
        }

        printf("Can't place mech: invalid position (%d,%d,%d), ret_code= %d \n", x,y,z,ret);
        return false;
    }

    struct Mech m;
    m.type = type;
    m.subtype = subtype;
    m.x = x;
    m.y = y;
    m.z = z;
    m.growth_ttl = mech_attributes[type].growth_ttl;

    m.side = 255;
    class MechAttribute* ma = get_mech_attribute(type);
    switch (ma->class_type)
    {
        case MECH_CRYSTAL:
        case MECH_CROP:
        case MECH_MYCELIUM:
            break;
        case MECH_SIGN:
            if(t_map::isSolid(x-1,y,z))
            {
                m.side = 2;
            }
            else if(t_map::isSolid(x+1,y,z))
            {
                m.side = 3;
            } 
            else if(t_map::isSolid(x,y+1,z))
            {
                m.side = 4;
            }
            else if(t_map::isSolid(x,y-1,z))
            {
                m.side = 5;
            }
            break;
        case MECH_WIRE:
        case MECH_SWITCH:
        case NULL_MECH_CLASS:
            GS_ASSERT(false);
            return false;
    }


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
    if (t_map::isSolid(x,y,z)) return false;
    if (mech_list->is_occupied(x,y,z)) return false;

    return true;
}

//returns 0 if true
int can_place_mech(int x, int y, int z, MechType mech_type, int side)
{
    if (z <= 0 || z > 128) return 1;
    if (side != 0) return false;

    if (t_map::isSolid(x,y,z)) return 2;

    class MechAttribute* ma = get_mech_attribute(mech_type);
    switch (ma->class_type)
    {
        case MECH_CRYSTAL:
        case MECH_CROP:
        case MECH_MYCELIUM:
            if (!t_map::isSolid(x,y,z-1)) return 3;
            if (mech_list->is_occupied(x,y,z)) return 4;
            break;
        case MECH_SIGN:
            if (mech_list->is_occupied(x,y,z)) return 5;

            if( !t_map::isSolid(x-1,y,z) &&
                !t_map::isSolid(x+1,y,z) &&
                !t_map::isSolid(x,y+1,z) &&
                !t_map::isSolid(x,y-1,z) )
            {
                //return 6;
            }
            break;
        case MECH_WIRE:
        case MECH_SWITCH:
        case NULL_MECH_CLASS:
            GS_ASSERT(false);
            return 6;
    }

    return 0;
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
