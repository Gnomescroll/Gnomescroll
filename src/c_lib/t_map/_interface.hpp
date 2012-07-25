#pragma once

namespace t_map
{
void end_t_map();

int get_block_item_container(int x, int y, int z);

#if DC_CLIENT
extern class Vbo_map* vbo_map;

void init_t_vbo();
void end_t_vbo();

void draw_map();
void draw_map_compatibility();
void update_map();


void control_node_render_init(class CONTROL_NODE_LIST* _cnl);	//internal method
void control_node_render_teardown();	//internal method

void control_node_render_update();
void control_node_render_draw();

// block damage info
void request_block_damage(int x, int y, int z);
void received_block_damage_response(unsigned int request_id, unsigned int dmg);
int get_requested_block_remaining_health();
bool is_last_requested_block(int x, int y, int z);
extern unsigned int requested_block_damage;
#endif

#if DC_SERVER
void create_item_container_block(int x, int y, int z, int container_type, int container_id);
void destroy_item_container_block(int x, int y, int z);
void get_container_location(int container_id, int position[3]);
//server/env_process.hpp
void map_post_processing();

//on connect
void send_client_map_special(int client_id);
void add_control_node(int x, int y, int z);
#endif
}
