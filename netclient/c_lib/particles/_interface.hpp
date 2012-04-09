#pragma once

extern class Cspray_list* cspray_list;
extern class Grenade_list* grenade_list;

#ifdef DC_CLIENT
extern class  Cspray_list* cspray_list;
extern class  Grenade_list* grenade_list;

extern class  Shrapnel_list* shrapnel_list;
extern class  Blood_list* blood_list;
extern class  Minivox_list* minivox_list;
extern class  BillboardText_list* billboard_text_list;
extern class  BillboardTextHud_list* billboard_text_hud_list;
#endif

#ifdef DC_SERVER
extern class  Grenade_shrapnel_list* grenade_shrapnel_list;
#endif

void init_particles();
void tear_down_particles();