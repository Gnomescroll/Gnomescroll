/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
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

class MechListMeshRenderer* mech_list_mesh_renderer = NULL;
class MechLightEffect* mech_light_effect_renderer = NULL;

#endif

void init_packets()
{
    mech_create_StoC::register_client_packet();
    mech_type_change_StoC::register_client_packet();
    mech_delete_StoC::register_client_packet();

    mech_text_StoC::register_client_packet();
    mech_text_update_StoC::register_client_packet();
}

void init()
{
    GS_ASSERT(mech_list == NULL);
    mech_list = new MechList;
    #if DC_CLIENT
    GS_ASSERT(mech_list_renderer == NULL);
    mech_list_renderer = new MechListRenderer;
    mech_list_mesh_renderer = new MechListMeshRenderer;
    mech_light_effect_renderer = new MechLightEffect;
    // update the mech attribute dimensions after we determine sprite dimensions
    // TODO -- handle this on server too?
    update_dimensions();
    #endif
}

void teardown()
{
    delete mech_list;
    #if DC_CLIENT
    delete mech_list_renderer;
    delete mech_list_mesh_renderer;
    delete mech_light_effect_renderer;
    #endif
}

#if DC_CLIENT
void prep()
{
    mech_list_renderer->prep_vbo();
    mech_list_mesh_renderer->prep();
    mech_light_effect_renderer->prep();
}

void draw()
{
    mech_list_renderer->draw();
    mech_list_mesh_renderer->draw();
}

void draw_transparent()
{
    //mech_list_renderer->draw_transparent();
    mech_light_effect_renderer->draw_transparent();
}
#endif


#if DC_SERVER
void pack_mech(const struct Mech &m, class mech_create_StoC &p)
{
    p.id = m.id;
    p.type = m.type;
    p.subtype = m.subtype;
    p.side = m.side;
    p.position = m.position;

    GS_ASSERT(m.side >= 0 && m.side <= 5);

    switch (get_mech_class(m.type))
    {
        case MECH_CRYSTAL:
            break;
        case MECH_CROP:
            break;
        case MECH_MYCELIUM:
            break;
        case MECH_SIGN:
            break;
        case MECH_WALL_OBJECT:
            break;
        case MECH_WIRE:
        case MECH_SWITCH:
        case NULL_MECH_CLASS:
        default:
            printf("pack_mech error: unhandled mech type\n");
            break;
    }
}
#endif


#if DC_CLIENT
//call after type or subtype changes
static bool update_mech(struct Mech &m)
{
    class MechAttribute* ma = get_mech_attributes(m.type);
    m.render_type = ma->render_type;

    float size = get_mech_size(m.type);

    m.rotation = 0.0f;
    m.offset = 0;
    m.offset_x = 0.0f;
    m.offset_y = 0.0f;

    switch (ma->class_type)
    {
        case MECH_CRYSTAL:
            //do something
            //m.render_type = ma->render_type;
            m.rotation = 0.25f*(rand()%4) + 0.25f*randf()/3;

            //m.rotation = 0.0f;
            m.offset = rand()%255;
            //m.subtype = rand()%6;

            m.offset_x = (randf()-0.5f)* (1.0f-size);
            m.offset_y = (randf()-0.5f)* (1.0f-size);

            m.offset_x = 0.0f;
            m.offset_y = 0.0f;
            break;

        case MECH_CROP:
        case MECH_MYCELIUM:
            break;

        case MECH_SIGN:
            break;

        case MECH_WALL_OBJECT:
            m.rotation = 0.0f;
            m.offset = 0;
            m.offset_x = 0.0f;
            m.offset_y = 0.0f;
            break;

        case MECH_WIRE:
        case MECH_SWITCH:
        case NULL_MECH_CLASS:
            GS_ASSERT(false);
            return false;
    }

    m.center = get_mech_center(m);

    return true;
}

static bool unpack_mech(struct Mech &m, const mech_create_StoC &p)
{
    IF_ASSERT(!isValid((MechType)p.type)) return false;
    m.id = p.id;
    m.type = (MechType)p.type;
    m.subtype = p.subtype;
    m.side = p.side;
    m.position = p.position;
    m.offset_x = 0;
    m.offset_y = 0;
    m.render_type = MECH_RENDER_TYPE_0;
    m.text = NULL;
    return update_mech(m);
    m.center = get_mech_center(m);
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
        MechBehaviorType behavior_type = get_mech_behavior_type(type);

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
                light_value = t_map::get_skylight(mla[i].position);
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
        MechBehaviorType behavior_type = get_mech_behavior_type(type);

        int x = mla[i].position.x;
        int y = mla[i].position.y;
        int z = mla[i].position.z;

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

    MechType growth_stage = get_mech_growth_stage(type);  //next growth stage

    GS_ASSERT(isValid(growth_stage));
    GS_ASSERT(mla[mech_id].id != -1);
    GS_ASSERT(mech_id >= 0 && mech_id < mlm);

    mla[mech_id].type =  growth_stage;
    mla[mech_id].growth_ttl = get_mech_growth_ttl(growth_stage);

    class mech_type_change_StoC p;
    p.id = mech_id;
    p.type = growth_stage;
    p.broadcast();
}

void print_mech_create_failure_code(MechCreateFailureCode code)
{
    if (code == MCF_OK) return;
    printf("Failed to create mech. Reason: ");
    switch (code)
    {
        case MCF_BAD_TYPE:
            printf("Bad mech type");
            break;
        case MCF_NOT_USED:
            printf("Mech type not in use");
            break;
        case MCF_BAD_Z:
            printf("Z out of range");
            break;
        case MCF_BAD_SIDE:
            printf("Invalid side");
            break;
        case MCF_SOLID_BLOCK:
            printf("Destination point was a solid block");
            break;
        case MCF_ON_AIR:
            printf("Destination point was not above a solid block");
            break;
        case MCF_OCCUPIED:
            printf("A mech was already here");
            break;
        case MCF_UNHANDLED:
            printf("Mech type is currently unhandled for creation");
            break;
        case MCF_NO_WALL:
            printf("There is no wall for the mech to attach to");
            break;
        case MCF_FULL:
            printf("Mech list is full");
            break;
        case MCF_OTHER:
            printf("Unspecified");
        case MCF_OK:
            printf("Nevermind, no failure.");
            break;
        default:
            printf("UNKNOWN FAILURE CODE\n");
            break;
    }
    printf("\n");
}

MechCreateFailureCode create_mech(const Vec3i& position, MechType type, int side)
{
    MechCreateFailureCode ret = can_place_mech(position, type, side);
    if (ret != MCF_OK) return ret;

    struct Mech m;
    m.render_type = get_mech_render_type(type);
    m.type = type;
    m.subtype = 0;
    m.position = position;
    m.growth_ttl = get_mech_growth_ttl(type);
    m.side = side;
    m.text = NULL;
    m.offset = 0;
    m.offset_x = 0.0f;
    m.offset_y = 0.0f;
    m.center = get_mech_center(m);

    MechClassType class_type = get_mech_class(m.type);
    switch (class_type)
    {
        case MECH_CRYSTAL:
        case MECH_CROP:
        case MECH_MYCELIUM:
        case MECH_SIGN:
        case MECH_WALL_OBJECT:
            break;
        case MECH_WIRE:
        case MECH_SWITCH:
        case NULL_MECH_CLASS:
            GS_ASSERT(false);
            return MCF_UNHANDLED;
    }

    if (class_type == MECH_SIGN)
    {
        m.text = calloc(MECH_TEXT_SIZE_MAX+1, 1);
        char inc = '0';
        for (int i=0; i < MECH_TEXT_SIZE_MAX; i++, inc++)
            ((char*)m.text)[i] = inc;
    }

    static const MechType light_crystal = t_mech::get_mech_type("light_crystal");
    if (isValid(light_crystal) && type == light_crystal)
        t_map::broadcast_set_block(position, "mech_light_empty");

    if (!mech_list->server_add_mech(m))
        return MCF_FULL;
    return MCF_OK;
}

MechCreateFailureCode create_mech(const Vec3i& position, MechType type)
{
    return create_mech(position, type, NULL_MECH_SIDE);
}

MechCreateFailureCode create_crystal(const Vec3i& position, MechType type)
{
    MechClassType mech_class = get_mech_class(type);
    IF_ASSERT(mech_class != MECH_CRYSTAL) return MCF_OTHER;
    return create_mech(position, type);
}
#endif


MechType get_mech_type(int mech_id)
{
    struct Mech* m = mech_list->get(mech_id);
    if (m == NULL) return NULL_MECH_TYPE;
    return m->type;
}

MechCreateFailureCode can_place_mech(const Vec3i& position, MechType mech_type, int side)
{
    if (!is_valid_z(position) || position.z == 0) return MCF_BAD_Z;
    if (t_map::isSolid(position)) return MCF_SOLID_BLOCK;
    MechClassType class_type = get_mech_class(mech_type);
    int x = position.x;
    int y = position.y;
    int z = position.z;
    switch (class_type)
    {
        case MECH_CRYSTAL:
        case MECH_CROP:
        case MECH_MYCELIUM:
            if (!t_map::isSolid(x,y,z-1))
                return MCF_ON_AIR;
            if (mech_list->is_occupied(position))
                return MCF_OCCUPIED;
            break;
        case MECH_SIGN:
            if (mech_list->is_occupied(position))
                return MCF_OCCUPIED;
            if (side == 0 && !t_map::isSolid(x,y,z-1))
                return MCF_NO_WALL;
            if (side == 1 && !t_map::isSolid(x,y,z+1))
                return MCF_NO_WALL;
            if (side == 2 && !t_map::isSolid(x-1,y,z))
                return MCF_NO_WALL;
            if (side == 3 && !t_map::isSolid(x+1,y,z))
                return MCF_NO_WALL;
            if (side == 4 && !t_map::isSolid(x,y-1,z))
                return MCF_NO_WALL;
            if (side == 5 && !t_map::isSolid(x,y+1,z))
                return MCF_NO_WALL;

            break;
        case MECH_WALL_OBJECT:
            if (mech_list->is_occupied(position))
                return MCF_OCCUPIED;
            if (side == 0 && !t_map::isSolid(x,y,z-1))
                return MCF_NO_WALL;
            if (side == 1 && !t_map::isSolid(x,y,z+1))
                return MCF_NO_WALL;
            if (side == 2 && !t_map::isSolid(x-1,y,z))
                return MCF_NO_WALL;
            if (side == 3 && !t_map::isSolid(x+1,y,z))
                return MCF_NO_WALL;
            if (side == 4 && !t_map::isSolid(x,y-1,z))
                return MCF_NO_WALL;
            if (side == 5 && !t_map::isSolid(x,y+1,z))
                return MCF_NO_WALL;
        case MECH_WIRE:
            break;
        case MECH_SWITCH:
        case NULL_MECH_CLASS:
            GS_ASSERT(false);
            return MCF_UNHANDLED;
    }
    return MCF_OK;
}

MechCreateFailureCode can_place_mech(const Vec3i& position, MechType mech_type)
{
    return can_place_mech(position, mech_type, 0);
}


#if DC_SERVER

void send_client_mech_list(ClientID client_id)
{
    mech_list->send_mech_list_to_client(client_id);
}

void handle_block_removal(const Vec3i& position)
{
    MechType type = mech_list->handle_block_removal(position);
    if (type == NULL_MECH_TYPE) return;
    IF_ASSERT(!isValid(type)) return;
    // drop item from mech
    if (has_item_drop(type))
        handle_drop(position, type);
}

bool remove_mech(int mech_id)   //removes mech with drop
{
    IF_ASSERT(mech_id < 0 || mech_id >= mech_list->mlm) return false;
    struct Mech m = mech_list->mla[mech_id];
    MechType type = m.type;
    IF_ASSERT(!isValid(type)) return false;

    bool ret = mech_list->server_remove_mech(mech_id);
    GS_ASSERT(ret);

    if (has_item_drop(type))
        handle_drop(m.position, type);

    return ret;
}

int count_mech(MechType type)
{
    return mech_list->count(type);
}

void hit_mech(int mech_id)
{   // TODO -- custom handling for mech getting hit
    remove_mech(mech_id);
}

#endif

//    remove_mech(mech_id);
//}

char* get_mech_text(int mech_id)
{
    GS_ASSERT(mech_id >= 0 && mech_id < mech_list->mlm);
    return reinterpret_cast<char*>(mech_list->mla[mech_id].text);
}

#if DC_CLIENT
//updates text on
void mech_text_update(int mech_id, int pos, int key)
{
    GS_ASSERT(mech_id >= 0 && mech_id < mech_list->mlm);
    GS_ASSERT(mech_list->mla[mech_id].id != -1);
    GS_ASSERT(get_mech_attributes(mech_list->mla[mech_id].type)->class_type == MECH_SIGN);
    IF_ASSERT(mech_list->mla[mech_id].text != NULL) return;
    IF_ASSERT(pos < MECH_TEXT_SIZE_MAX) return;
    //send packet

}
#endif

}   // t_mech
