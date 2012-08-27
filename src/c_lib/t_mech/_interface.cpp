#include "_interface.hpp"

#include <t_mech/net/CtoS.hpp>
#include <t_mech/net/StoC.hpp>

#if DC_CLIENT
#include <camera/camera.hpp>
#include <physics/ray_trace/hitscan.hpp>
#include <t_mech/mech_draw.hpp>
#endif

#include <t_mech/mech_state.hpp>


namespace t_mech
{

class MECH_LIST* mech_list;

void init_packets()
{
    mech_create_StoC::register_client_packet();
    mech_delete_StoC::register_client_packet();
}



#if DC_CLIENT
class MechListRenderer* mech_list_renderer = NULL;
#endif

void init() 
{
    mech_list = new MECH_LIST;
    #if DC_CLIENT
    mech_list_renderer = new MechListRenderer;
    #endif
    init_properties();
}

void teardown() 
{
    tear_down_properties();
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

/*
    MECH_CRYSTAL,
    MECH_CROP,
    MECH_WIRE,
    MECH_SWITCH
*/

/*
    MECH_CRYSTAL,
    MECH_CROP,
    MECH_WIRE,
    MECH_SWITCH
*/

/*
struct MECH_ATTRIBUTE
{
    int mech_type;
    int mech_type_class;
    int render_type;
    int sprite_index;
};
*/

//pack mech data into packet
#if DC_SERVER
static void pack_mech(struct MECH &m, class mech_create_StoC &p)
{
    p.id = m.id;
    p.mech_type = m.mech_type;
    p.subtype = m.subtype;
    p.x = m.x;
    p.y = m.y;
    p.z = m.z;

    GS_ASSERT(mech_attribute[m.mech_type].mech_type != -1);

    switch ( mech_attribute[m.mech_type].mech_type_class)
    {
    case MECH_CRYSTAL:
        break;
    case MECH_CROP:
        break;
    case MECH_MYCELIUM:
        break;
    default:
        printf("pack_mech error: unhandled mech type\n");
    }
}
#endif


//handles unpacking
#if DC_CLIENT
static void unpack_mech(struct MECH &m, class mech_create_StoC &p)
{
    m.id = p.id;
    m.mech_type = p.mech_type;
    m.subtype = p.subtype;
    m.x = p.x;
    m.y = p.y;
    m.z = p.z;

    struct MECH_ATTRIBUTE* ma = get_mech_attribute(p.mech_type); 

    switch ( ma->mech_type_class )
    {
    case MECH_CRYSTAL:
        //do something
        m.render_type = ma->render_type;

        m.size = 0.80;  //radius
        m.rotation = 0.25*(rand()%4) + 0.25f*randf()/3;
        m.offset = rand()%255;
        //m.subtype = rand()%6;

        m.offset_x = (randf()-0.5f)* (1.0f-m.size);
        m.offset_y = (randf()-0.5f)* (1.0f-m.size);
        break;
    case MECH_CROP:
        break;
    case MECH_MYCELIUM:
        break;
    default:
        GS_ASSERT(false);
        printf("pack_mech error: unhandled mech type\n");
        break;
    } 
}
#endif


#if DC_SERVER
void create_crystal(int x, int y, int z, int mech_type)
{
    ASSERT_VALID_MECH_TYPE(mech_type);
    IF_INVALID_MECH_TYPE(mech_type) return;
    GS_ASSERT(get_mech_class(mech_type) == MECH_CRYSTAL);

    GS_ASSERT(can_place_crystal(x,y,z,0));
    if(!can_place_crystal(x,y,z, 0)) return;

    GS_ASSERT(mech_attribute[mech_type].mech_type != -1);
    if (mech_attribute[mech_type].mech_type == -1)
    {
        printf("t_mech::create_crystal fail: mech_type %i does not exist \n",  mech_type);
        return;
    }

    struct MECH m;
    m.mech_type = mech_type;
    m.x = x;
    m.y = y;
    m.z = z;

    mech_list->server_add_mech(m);
}
#endif

bool can_place_crystal(int x, int y, int z, int side)
{
    if (z <= 0 || z > 128) return false;
    if (side != 0) return false;
    
    if (isSolid(x,y,z)) return false;
    if (!isSolid(x,y,z-1)) return false;

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

bool ray_cast_mech_render_type_0(const class MECH &m, float x, float y, float z, float vx, float vy, float vz, float* distance)
{
    const float size2 = m.size/2.0f;
    float wx = (float) (m.x) + 0.5f + m.offset_x;
    float wy = (float) (m.y) + 0.5f + m.offset_y;
    float wz = (float) m.z + size2;

    wx = quadrant_translate_f(current_camera_position.x, wx);
    wy = quadrant_translate_f(current_camera_position.y, wy);

    //float dx = sin(m.rotation * PI);
    //float dy = cos(m.rotation * PI);

    //translate into origin of plane
    x -= wx;
    y -= wy;
    z -= wz;

    return true;
/*
    float _x 
    m.size
    m.rotation
    m.offset_x
    m.offset_y
*/
}

bool ray_cast_mech(float x, float y, float z, float vx, float vy, float vz, float* _distance)
{
    //int nearest_mech = -1;
    //float distance = 1000.0;

    const int mlm = mech_list->mlm;
    const struct MECH* mla = mech_list->mla;

    for(int i=0; i<mlm; i++)
    {
        if( mla[i].id == -1) continue;

        float d;
        bool ret;

        switch ( mla[i].render_type )
        {
        case MECH_RENDER_TYPE_0: //MECH_CRYSTAL:
            //do something
            ret = ray_cast_mech_render_type_0(mla[i], x,y,z, vx,vy,vz, &d);
            if(ret == true)
            {
                printf("mech raycast hit: %i \n", i);
                return true;
            }
            break;
        default:
            printf("pack_mech error: unhandled mech type\n");
        }
    }

    return false;
}
#endif

#if DC_SERVER

void send_client_mech_list(int client_id)
{
    mech_list->send_mech_list_to_client(client_id);
}

void handle_block_removal(int x, int y, int z)
{
    int mech_type = mech_list->handle_block_removal(x,y,z);
    IF_INVALID_MECH_TYPE(mech_type) return;

    // drop item from mech
    if(mech_attribute[mech_type].item_drop) 
        handle_drop(x,y,z, mech_type);
}


#endif

}   // t_mech
