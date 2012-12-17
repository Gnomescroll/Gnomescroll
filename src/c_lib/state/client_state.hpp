#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <agent/constants.hpp>
#include <entity/objects.hpp>
#include <physics/vec3.hpp>
#include <agent/client/player_agent.hpp>
#include <voxel/voxel_hitscan.hpp>
#include <voxel/voxel_render.hpp>

namespace ClientState
{

extern int tick_id;
extern int frame_id;

extern struct Vec3 location_pointer;
extern bool location_pointer_set;

extern class PlayerAgent_state playerAgent_state;

// property lists
extern class Voxel_render_list_manager* voxel_render_list;
extern class Voxel_hitscan_list* voxel_hitscan_list;

extern int last_ping_time;
extern int last_reliable_ping_time;

extern class Entities::Entity* base;

void set_PlayerAgent_id(AgentID id);

void init_lists();
void teardown_voxel_lists();

// connect callback
void on_connect();
void on_disconnect();

void send_version();

void update_camera();
void send_camera_state();

void set_location_pointer();

}   // ClientState
