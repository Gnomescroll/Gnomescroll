#pragma once

#include <t_map/t_properties.hpp>
#include <t_map/common/map_pos.hpp>
#include <t_map/utils.hpp>

namespace t_map
{

extern struct Vec3i map_dim;

void init_t_map();
void end_t_map();

void init_packets();

ItemContainerID get_block_item_container(int x, int y, int z);
bool get_container_location(ItemContainerID container_id, int position[3]); // returns false is container not found

CubeType get(int x, int y, int z);
CubeType get(const struct Vec3i& pos);
int get_block_damage(int x, int y, int z);
struct MapElement get_element(int x, int y, int z);

CubeType set(int x, int y, int z, CubeType cube_type);
CubeType set(const struct Vec3i& pos, CubeType cube_type);
void set_fast(int x, int y, int z, CubeType cube_type);
void set_palette(int x, int y, int z, int palette);
void set_element(int x, int y, int z, struct MapElement e);

void _sunlight_update_core();   //update sunlight
void _envlight_update_core();   //update envlight
void lighting_rolling_update(int max_updates); //rolling update of lighting
void post_gen_map_lighting(); //run after map gen

#if DC_CLIENT
extern class VBOMap* vbo_map;

void init_for_draw();
void init_shaders();

void init_t_vbo();
void teardown_t_vbo();

void draw_map();
void draw_map_compatibility();
void update_map();

void control_node_render_init(class ControlNodeList* _cnl);   //internal method
void control_node_render_teardown();    //internal method

void control_node_render_update();
void control_node_render_draw();

// block damage info
void request_block_damage(int x, int y, int z);
void received_block_damage_response(unsigned int request_id, unsigned int dmg);
int get_requested_block_remaining_health();
void get_requested_block_position(int* x, int* y, int* z);
bool is_last_requested_block(int x, int y, int z);
extern unsigned int requested_block_damage;
extern CubeType requested_cube_type;
#endif

#if DC_SERVER
bool create_item_container_block(int x, int y, int z, ItemContainerType container_type, ItemContainerID container_id);

void smelter_on(ItemContainerID container_id);
void smelter_off(ItemContainerID container_id);

//server/env_process.hpp
void map_post_processing();

//on connect
void send_client_map_special(ClientID client_id);
void add_control_node(int x, int y, int z);

void apply_damage_broadcast(int x, int y, int z, int dmg, TerrainModificationAction action);

void broadcast_set_block_action(int x, int y, int z, CubeType cube_type, int action);
bool broadcast_set_block(int x, int y, int z, CubeType cube_type);
void broadcast_set_block_palette(int x, int y, int z, CubeType cube_type, int palette);
void broadcast_set_palette(int x, int y, int z, int palette);

void handle_explosive_block(int x, int y, int z);

int get_rad_level(int x, int y, int z);

/*
    Lighting
*/
int get_skylight(int x, int y, int z);
#endif

}   // t_map
