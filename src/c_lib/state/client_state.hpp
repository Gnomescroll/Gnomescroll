#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <agent/agent_list.hpp>
#include <entity/objects.hpp>

class PlayerAgent_state; //forward declaration

namespace ClientState
{

extern int tick_id;
extern int frame_id;

extern Vec3 location_pointer;
extern bool location_pointer_set;

extern PlayerAgent_state playerAgent_state;

extern class Agent_list* agent_list;

// property lists
extern class Voxel_render_list_manager* voxel_render_list;
extern class Voxel_hitscan_list* voxel_hitscan_list;

extern int last_ping_time;
extern int last_reliable_ping_time;

extern Objects::Object* base;

void set_PlayerAgent_id(int id);

void update_for_draw();

void init_lists();
void teardown_lists();

// connect callback
void on_connect();
void on_disconnect();

void send_version();

void update_camera();
void send_camera_state();

void set_location_pointer();

}   // ClientState
