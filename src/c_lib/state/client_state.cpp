#include "client_state.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <agent/agent_list.hpp>
#include <voxel/voxel_render.hpp>
#include <hud/text.hpp>
#include <agent/agent.hpp>
#include <agent/net_agent.hpp>
#include <agent/client/player_agent.hpp>
#include <particle/_interface.hpp>
#include <item/particle/_interface.hpp>

#if USE_OPENAL
#include <sound/openal.hpp>
#endif

const int GAME_OBJECT_MAX = 4096 * 4;

namespace ClientState
{

int tick_id = 0;
int frame_id = 0;

Vec3 location_pointer = vec3_init(0,0,0);
bool location_pointer_set = false;

Agent_list* agent_list = NULL;

Voxel_render_list_manager* voxel_render_list;
Voxel_hitscan_list* voxel_hitscan_list = NULL;

int last_ping_time;
int last_reliable_ping_time;

PlayerAgent_state playerAgent_state;

int agent_control_state[16];

void init_lists()
{
    voxel_render_list = new Voxel_render_list_manager;
    voxel_render_list->init();
    voxel_hitscan_list = new Voxel_hitscan_list;

    agent_list = new Agent_list;
}

void teardown_lists()
{
    // voxel models
    if (agent_list != NULL) delete agent_list;

    // voxel lists
    // must go after all voxels
    if (voxel_render_list != NULL) delete voxel_render_list;
    if (voxel_hitscan_list != NULL) delete voxel_hitscan_list;
}

void set_PlayerAgent_id(int id)
{
    playerAgent_state.set_PlayerAgent_id(id);
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
}

void update_for_draw()
{
    agent_list->update_models();
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
    if (playerAgent_state.you == NULL) return;
    if (!playerAgent_state.you->initial_teleport) return;
    agent_camera_state_CtoS msg;
    msg.id = playerAgent_state.you->client_id;
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
    chat_client->send_system_message("Disconnected from server");
}

void set_location_pointer()
{
    location_pointer_set = false;
    Vec3 loc = playerAgent_state.action.get_aiming_point();
    if (vec3_equal(loc, vec3_init(0,0,0))) return;
    location_pointer = loc;
    location_pointer_set = true;

    printf("Locator: %2.2f %2.2f %2.2f \n", loc.x, loc.y, loc.z);

    t_mech::tick(loc.x, loc.y, loc.z+0.2);
}

}   // ClientState
