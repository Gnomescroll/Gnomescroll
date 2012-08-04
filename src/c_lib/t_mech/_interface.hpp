#pragma once

namespace t_mech
{

extern class MECH_LIST* mech_list;

void init();
void teardown();

void init_packets();

#if DC_CLIENT

void prep();

void draw();

#endif

void create_crystal(int x, int y, int z);




void tick(int x, int y, int z);



#if DC_SERVER
#endif


}