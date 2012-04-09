#include "_interface.hpp"



#include "grenade.hpp"
#include "cspray.hpp"

class Grenade_list* grenade_list = NULL;
class Cspray_list* cspray_list = NULL;


#ifdef DC_CLIENT
#include <c_lib/particles/shrapnel.hpp>
#include <c_lib/particles/blood.hpp>
#include <c_lib/particles/minivox.hpp>
#include <c_lib/particles/billboard_text.hpp>
#include <c_lib/particles/billboard_text_hud.hpp>
#include <c_lib/particles/billboard_sprite.hpp>

class Cspray_list* cspray_list = NULL;
class Grenade_list* grenade_list = NULL;
class Shrapnel_list* shrapnel_list = NULL;
class Blood_list* blood_list = NULL;
class Minivox_list* minivox_list = NULL;
class BillboardText_list* billboard_text_list = NULL;
class BillboardTextHud_list* billboard_text_hud_list = NULL;

#endif

#if DC_SERVER
class Grenade_shrapnel_list* grenade_shrapnel_list = NULL;
#endif

void init_particles()
{
    cspray_list = new Cspray_list;
    grenade_list = new Grenade_list;

#ifdef DC_CLIENT
    cspray_list = new Cspray_list;
    grenade_list = new Grenade_list;
    shrapnel_list = new Shrapnel_list;
    blood_list = new Blood_list;
    //neutron_list = new Neutron_list;
    minivox_list = new Minivox_list;
    billboard_text_list = new BillboardText_list;
    billboard_text_hud_list = new BillboardTextHud_list;
#endif

#ifdef DC_SERVER
    grenade_shrapnel_list = new Grenade_shrapnel_list;
#endif


}

void tear_down_particles()
{
	delete cspray_list;
	delete grenade_list;

#ifdef DC_CLIENT
    delete shrapnel_list;
    delete blood_list;
    delete minivox_list;
    delete billboard_text_list;
    delete billboard_text_hud_list;
#endif

#ifdef DC_SERVER
    delete grenade_shrapnel_list;
#endif
}


