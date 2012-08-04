#include "_interface.hpp"


#if DC_CLIENT
    #include <camera/camera.hpp>
    #include <physics/ray_trace/hitscan.hpp>
	#include <t_mech/mech_draw.hpp>
#endif

#include <t_mech/mech_state.hpp>

namespace t_mech
{

class MECH_LIST* mech_list;


struct _MECH
{
    int x,y,z; //position
    int type;  //type
    int direction; //direction

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

void create_crystal(int x, int y, int z)
{
	mech_list->add_mech(x,y,z, 0);
}




void tick(int x, int y, int z)
{
#ifdef DC_CLIENT
    printf("create crystal: %d %d %d \n", x,y,z);
    create_crystal(x,y,z);
#endif
}



#if DC_SERVER
#endif


}