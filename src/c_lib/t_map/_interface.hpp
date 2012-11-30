#pragma once

namespace t_map
{

extern struct MapDimension map_dim;

void init_t_map();
void end_t_map();

void init_packets();

int get_block_item_container(int x, int y, int z);
bool get_container_location(int container_id, int position[3]); // returns false is container not found

CubeID get(int x, int y, int z);
void set(int x, int y, int z, CubeID cube_id);
inline void set_fast(int x, int y, int z, CubeID cube_id) __attribute__((always_inline));
void set_palette(int x, int y, int z, int palette);

int get_block_damage(int x, int y, int z);

void update_skylight(int chunk_i, int chunk_j); //update skylighting for chunk

inline int get_highest_open_block(int x, int y, int vertical_gap);
inline int get_highest_open_block(int x, int y);
inline int get_nearest_open_block(int x, int y, int z, int vertical_gap); 
inline int get_nearest_open_block(int x, int y, int z); 
inline int get_lowest_open_block(int x, int y, int n);
inline int get_highest_solid_block(int x, int y);
inline int get_highest_solid_block(int x, int y, int z);
inline int get_lowest_solid_block(int x, int y);
inline int get_solid_block_below(int x, int y, int z);

inline bool position_is_loaded(int x, int y) __attribute__((always_inline));

bool block_can_be_placed(int x, int y, int z, CubeID cube_id);


#if DC_CLIENT
extern class Vbo_map* vbo_map;

void init_for_draw();
void init_shaders();

void init_t_vbo();
void teardown_t_vbo();

void draw_map();
void draw_map_compatibility();
void update_map();

void control_node_render_init(class CONTROL_NODE_LIST* _cnl);   //internal method
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
extern CubeID requested_cube_id;
#endif

#if DC_SERVER
void create_item_container_block(int x, int y, int z, ItemContainerType container_type, int container_id);
void destroy_item_container_block(int x, int y, int z);

void smelter_on(int container_id);
void smelter_off(int container_id);

//server/env_process.hpp
void map_post_processing();

//on connect
void send_client_map_special(ClientID client_id);
void add_control_node(int x, int y, int z);

void apply_damage_broadcast(int x, int y, int z, int dmg, TerrainModificationAction action);

void broadcast_set_block_action(int x, int y, int z, CubeID cube_id, int action);
void broadcast_set_block(int x, int y, int z, CubeID cube_id);
void broadcast_set_block_palette(int x, int y, int z, CubeID cube_id, int palette);
void broadcast_set_palette(int x, int y, int z, int palette);

void handle_explosive_block(int x, int y, int z);
#endif

}   // t_map
