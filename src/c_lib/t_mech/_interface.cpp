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




struct _MECH
{
    int x,y,z; //position
    int type;  //type
    int direction; //direction

    float size;
    float rotation;
    int offset;

    bool active;
};


#if DC_CLIENT
class MechListRenderer* mech_list_renderer = NULL;
#endif

void init() 
{
	mech_list = new MECH_LIST;
#if DC_CLIENT
	mech_list_renderer = new MechListRenderer;
#endif
}

void teardown() {}

void init_packets();

#if DC_CLIENT

void prep()
{
	mech_list_renderer->prep_vbo();
}

void draw()
{
	mech_list_renderer->draw();
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
    

//pack mech data into packet
static void pack_mech(struct MECH &m, class mech_create_StoC &p)
{
#if DC_SERVER
    p.id = m.id;
    p.type = m.type;
    p.x = m.x;
    p.y = m.y;
    p.z = m.z;

    switch ( m.type )
    {
    case MECH_CRYSTAL:
        //do something
        break;
    default:
        printf("pack_mech error: unhandled mech type\n");
    }
#endif
}


//handles unpacking
static void unpack_mech(struct MECH &m, class mech_create_StoC &p)
{
#if DC_CLIENT
    m.id = p.id;
    m.type = p.type;
    m.x = p.x;
    m.y = p.y;
    m.z = p.z;

    switch ( p.type )
    {
    case MECH_CRYSTAL:
        //do something

    	m.size = 0.25;	//radius
    	m.rotation = 0.25 + randf();
    	m.offset = rand()%255;

    	m.offset_x = 2.0f*(randf()-0.5f)* (0.5f-m.size);
       	m.offset_y = 2.0f*(randf()-0.5f)* (0.5f-m.size);

        break;
    default:
        printf("pack_mech error: unhandled mech type\n");
    } 
#endif
}


void create_crystal(int x, int y, int z)
{
#if DC_SERVER

	struct MECH m;
	m.type = MECH_CRYSTAL;
	m.x = x;
	m.y = y;
	m.z = z;

	mech_list->server_add_mech(m);
#endif
}

void tick(int x, int y, int z)
{
#ifdef DC_CLIENT
    //printf("create crystal: %d %d %d \n", x,y,z);
    //create_crystal(x,y,z);
#endif
}

}
