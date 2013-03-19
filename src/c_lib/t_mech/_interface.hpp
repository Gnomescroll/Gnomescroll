#pragma once

#include <t_mech/net/StoC.hpp>
#include <t_mech/common/common.hpp>
#include <t_mech/mech_state.hpp>

namespace t_mech
{

extern struct MechList* mech_list;

void init();
void teardown();
void init_packets();

void tick(int x, int y, int z);
void place_vine(int x, int y, int z, int side);
MechCreateFailureCode can_place_mech(int x, int y, int z, MechType mech_type);
MechCreateFailureCode can_place_mech(int x, int y, int z, MechType mech_type, int side);
void print_mech_create_failure_code(MechCreateFailureCode code);

#if DC_CLIENT
void prep();
void draw();
void draw_transparent();

void draw_selected_mech_bounding_box();
bool ray_cast_mech(float x, float y, float z, float vx, float vy, float vz, int* _mech_id, float* _distance);

void client_ray_cast();

static bool _mech_update(struct Mech &m); //call after type or subtype changes
static bool unpack_mech(struct Mech &m, class mech_create_StoC &p);
#endif

#if DC_SERVER
void tick(); //update growth cycles
void floating_removal_tick(); //removes floating t_mech

void force_mech_growth(int mech_id); //forces growth cycle

MechCreateFailureCode create_crystal(int x, int y, int z, MechType type);
MechCreateFailureCode create_mech(int x, int y, int z, MechType type);
MechCreateFailureCode create_mech(int x, int y, int z, MechType type, int subtype);

void send_client_mech_list(ClientID client_id);
void handle_block_removal(int x, int y, int z); //react to block destruction
bool remove_mech(int mech_id);

int count_mech(MechType type);

static void pack_mech(struct Mech &m, class mech_create_StoC &p);
#endif

}   // t_mech
