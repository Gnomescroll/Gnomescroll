#include "_interface.hpp"

//c#include <c_lib/particles/cspray.hpp>
#include <c_lib/particles/grenade.hpp>

#if DC_CLIENT
#include <c_lib/particles/shrapnel.hpp>
#include <c_lib/particles/blood.hpp>
#include <c_lib/particles/minivox_colored.hpp>
#include <c_lib/particles/minivox_textured.hpp>
#include <c_lib/particles/billboard_text.hpp>
#include <c_lib/particles/billboard_text_hud.hpp>
#include <c_lib/particles/billboard_sprite.hpp>
#endif 

#if DC_SERVER
#include <c_lib/particles/grenade_shrapnel.hpp>
#endif


namespace Particles
{

class Grenade_list* grenade_list = NULL;
class Cspray_list* cspray_list = NULL;


#if DC_CLIENT
class Shrapnel_list* shrapnel_list = NULL;
class Blood_list* blood_list = NULL;
class ColoredMinivox_list* colored_minivox_list = NULL;
class TexturedMinivox_list* textured_minivox_list = NULL;
class BillboardText_list* billboard_text_list = NULL;
class BillboardTextHud_list* billboard_text_hud_list = NULL;
#endif

#if DC_SERVER
class Grenade_shrapnel_list* grenade_shrapnel_list = NULL;
#endif

void init_particles()
{
    //cspray_list = new Cspray_list;
    grenade_list = new Grenade_list;

#if DC_CLIENT
    shrapnel_list = new Shrapnel_list;
    blood_list = new Blood_list;
    //neutron_list = new Neutron_list;
    colored_minivox_list = new ColoredMinivox_list;
    textured_minivox_list = new TexturedMinivox_list;
    billboard_text_list = new BillboardText_list;
    billboard_text_hud_list = new BillboardTextHud_list;
#endif

#if DC_SERVER
    grenade_shrapnel_list = new Grenade_shrapnel_list;
#endif


}

void teardown_particles()
{
    //delete cspray_list;
    delete grenade_list;

#if DC_CLIENT
    delete shrapnel_list;
    delete blood_list;
    delete colored_minivox_list;
    delete textured_minivox_list;
    delete billboard_text_list;
    delete billboard_text_hud_list;
#endif

#if DC_SERVER
    delete grenade_shrapnel_list;
#endif
}


}
