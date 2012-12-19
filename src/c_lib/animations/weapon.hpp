#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

namespace Animations
{

void draw_equipped_voxel_item_other_agent(AgentID agent_id, int item_type);
void draw_equipped_sprite_item_other_agent(AgentID agent_id, int item_type);
void draw_equipped_voxelized_sprite_item_other_agent(AgentID agent_id, int item_type);

void draw_equipped_item(int item_type);
void begin_equipped_item_animation(int item_type, bool continuous);
void tick_equipped_item_animation();
void stop_equipped_item_animation();

void draw_placement_outline(int item_type);

// controls for adjusting the sprite
void move_focal_depth(float delta);            // distance from agent plane
void move_focal_vertical(float delta);        // vertical distance
void move_focal_horizontal(float delta);    // horizontal distance

void move_origin_vertical(float delta);         // vertical distance along agent plane
void move_origin_horizontal(float delta);    // horizontal distance along agent plane
void move_origin_depth(float delta);        // distance from agent plane
    
void print_sprite_alignment_config();

void dilate_equipped_sprite(float delta);

void init_weapon_sprite();

// gl wrappers
bool draw_voxel_gl_begin(GLint cull_mode);
void draw_voxel_gl_end();
bool draw_sprite_gl_begin();
void draw_sprite_gl_end();

}   // Animations
