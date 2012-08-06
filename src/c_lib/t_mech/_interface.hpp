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
void draw_transparent();

#endif

void handle_block_removal(int x, int y, int z);
void create_crystal(int x, int y, int z, int subtype);

bool can_place_crystal(int x, int y, int z, int side);


void tick(int x, int y, int z);



#if DC_SERVER
#endif


}