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

MechCreateFailureCode can_place_mech(const Vec3i& position, MechType mech_type);
MechCreateFailureCode can_place_mech(const Vec3i& position, MechType mech_type, int side);
void print_mech_create_failure_code(MechCreateFailureCode code);

MechType get_mech_type(int mech_id);

#if DC_CLIENT
void prep();
void draw();
void draw_transparent();

void draw_selected_mech_bounding_box();
bool ray_cast_mech(const Vec3& position, const Vec3& direction, int& mech_id, float& distance);

void client_ray_cast();

static bool update_mech(struct Mech &m); //call after type or subtype changes
static bool unpack_mech(struct Mech &m, const class mech_create_StoC &p);
#endif

#if DC_SERVER
void tick(); //update growth cycles
void floating_removal_tick(); //removes floating t_mech

void force_mech_growth(int mech_id); //forces growth cycle

MechCreateFailureCode create_crystal(const Vec3i& position, MechType type);
MechCreateFailureCode create_mech(const Vec3i& position, MechType type);
MechCreateFailureCode create_mech(const Vec3i& position, MechType type, int side);

void send_client_mech_list(ClientID client_id);
void handle_block_removal(const Vec3i& position); //react to block destruction
bool remove_mech(int mech_id);

int count_mech(MechType type);

void pack_mech(const struct Mech& m, class mech_create_StoC& p);

void hit_mech(int mech_id);
#endif

//text
char* get_mech_text(int mech_id);
#if DC_CLIENT
void mech_text_update(int mech_id, int pos, int key);
#endif
}   // t_mech
