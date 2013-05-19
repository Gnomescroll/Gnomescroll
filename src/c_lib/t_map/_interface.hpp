#pragma once

#include <t_map/t_properties.hpp>
#include <t_map/utils.hpp>

namespace t_map
{

void init_t_map();
void end_t_map();

void init_packets();

ItemContainerID get_block_item_container(Vec3i position);
bool get_container_location(ItemContainerID container_id, Vec3i& position); // returns false is container not found

CubeType get(const Vec3i& position);
CubeType get(int x, int y, int z);
int get_block_damage(const Vec3i& position);
struct MapElement get_element(const Vec3i& position);
inline struct MapElement get_element(int x, int y, int z);

CubeType set(const Vec3i& position, CubeType cube_type);
CubeType set(int x, int y, int z, CubeType cube_type);
void set_fast(const Vec3i& position, CubeType cube_type);
void set_fast(int x, int y, int z, CubeType cube_type);
void set_palette(const Vec3i& position, int palette);
inline void set_element(const Vec3i& position, struct MapElement e);
inline void set_element(int x, int y, int z, struct MapElement e);

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
void request_block_damage(const Vec3i& position);
void received_block_damage_response(unsigned int request_id, unsigned int dmg);
int get_requested_block_remaining_health();
Vec3i get_requested_block_position();
bool is_last_requested_block(const Vec3i& position);
extern unsigned int requested_block_damage;
extern CubeType requested_cube_type;

#endif

#if DC_SERVER
bool create_item_container_block(const Vec3i& position, ItemContainerType container_type, ItemContainerID container_id);

void smelter_on(ItemContainerID container_id);
void smelter_off(ItemContainerID container_id);

//server/env_process.hpp
void map_post_processing();

//on connect
void send_client_map_special(ClientID client_id);
void add_control_node(const Vec3i& position);

void apply_damage_broadcast(const Vec3i& position, int dmg, TerrainModificationAction action);
void apply_damage_broadcast(int x, int y, int z, int dmg, TerrainModificationAction action);

void broadcast_set_block_action(const Vec3i& position, CubeType cube_type, int action);
bool broadcast_set_block(const Vec3i& position, CubeType cube_type);
void broadcast_set_block_palette(const Vec3i& position, CubeType cube_type, int palette);
void broadcast_set_block_palette(int x, int y, int z, CubeType block, int palette);
void broadcast_set_palette(const Vec3i& position, int palette);

void handle_explosive_block(const Vec3i& position);

int get_rad_level(const struct Vec3i& pos);

#endif

int get_envlight(int x, int y, int z);
inline int get_envlight(const Vec3i& position);
int get_skylight(int x, int y, int z);
inline int get_skylight(const Vec3i& position);

//light_cycle.hpp
#if DC_CLIENT
unsigned int generate_clut_light_texture();
void generate_light_texture();
#endif
void update_day_cycle();

}   // t_map
