#include "client_state.hpp"

#if DC_SERVER
# error Do not include this file in the server
#endif

#include <hud/text.hpp>
#include <agent/agent.hpp>
#include <agent/net_agent.hpp>
#include <particle/_interface.hpp>
#include <item/particle/_interface.hpp>

#if USE_OPENAL
# include <sound/openal.hpp>
#endif

const int GAME_OBJECT_MAX = 4096 * 4;

namespace ClientState
{

struct Vec3i* path = NULL;
size_t path_len = 0;

int tick_id = 0;
int frame_id = 0;

Vec3 location_pointer = vec3_init(0,0,0);
bool location_pointer_set = false;

Voxels::VoxelRenderListManager* voxel_render_list;
Voxels::VoxelHitscanList* voxel_hitscan_list = NULL;

int last_ping_time;
int last_reliable_ping_time;

Agents::PlayerAgent player_agent;

int agent_control_state[16];

void init_lists()
{
    GS_ASSERT(voxel_render_list == NULL);
    GS_ASSERT(voxel_hitscan_list == NULL);
    voxel_render_list = new Voxels::VoxelRenderListManager;
    voxel_render_list->init();
    voxel_hitscan_list = new Voxels::VoxelHitscanList;
}

void teardown_voxel_lists()
{
    delete voxel_render_list;
    delete voxel_hitscan_list;
}

void set_PlayerAgent_id(AgentID id)
{
    player_agent.set_PlayerAgent_id(id);
}

void tick()
{
    ItemParticle::tick();

    Particle::grenade_list->tick();
    Particle::shrapnel_list->tick();
    Particle::blood_list->tick();
    Particle::colored_minivox_list->tick();
    Particle::textured_minivox_list->tick();
    Particle::billboard_text_list->tick();
    Particle::billboard_text_hud_list->tick();
    Particle::plasmagen_spur_list->tick();
}

void update_camera()
{
    if (input_state.camera_mode == INPUT_STATE_AGENT)
        use_agent_camera();
    else
        use_free_camera();

    update_agent_camera();
}

void send_camera_state()
{
    if (current_camera == NULL) return;
    class Agents::Agent* you = player_agent.you();
    if (you == NULL || !you->initial_teleport) return;
    agent_camera_state_CtoS msg;
    Vec3 p = current_camera->get_position();
    msg.x = p.x;
    msg.y = p.y;
    msg.z = p.z;
    msg.theta = current_camera->theta;
    msg.phi = current_camera->phi;
    msg.send();
}

void send_ping()
{
    int n = _GET_MS_TIME();

    ping_CtoS msg;
    msg.ticks = n;
    msg.send();

    ping_reliable_CtoS msg2;
    msg2.ticks = n;
    msg2.send();
}

void send_version()
{
    version_CtoS msg;
    msg.version = GS_VERSION;
    msg.send();
}

void on_connect()
{
    // connect message is printed after version match
}

void on_disconnect()
{
    Chat::send_system_message("Disconnected from server");
}

void set_location_pointer()
{
    location_pointer_set = false;
    Vec3 loc = player_agent.action.get_aiming_point();
    if (vec3_equal(loc, vec3_init(0,0,0))) return;
    location_pointer = translate_position(loc);
    location_pointer_set = true;

    printf("Locator: ");
    vec3_print(loc);
}

void set_location_pointer_open_block()
{
    location_pointer_set = false;
    struct Vec3i pos;
    float max_len = float(map_dim.x) / 2.0f;
    location_pointer_set = player_agent.nearest_open_block(max_len, pos);
    if (!location_pointer_set) return;
    printf("Locator (open block): ");
    vec3i_print(pos);
    location_pointer = vec3_init(pos);
}

}   // ClientState
