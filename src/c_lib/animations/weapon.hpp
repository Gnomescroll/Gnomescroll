#pragma once

namespace Animations
{

void draw_equipped_item(int item_type);
void begin_equipped_item_animation(int item_type, bool continuous);
void tick_equipped_item_animation();
void stop_equipped_item_animation();

// controls for adjusting the sprite
void move_focal_depth(float delta);			// distance from agent plane
void move_focal_vertical(float delta);		// vertical distance
void move_focal_horizontal(float delta);	// horizontal distance

void move_origin_vertical(float delta);	 	// vertical distance along agent plane
void move_origin_horizontal(float delta);	// horizontal distance along agent plane
void move_origin_depth(float delta);		// distance from agent plane
    
void print_sprite_alignment_config();

void dilate_equipped_sprite(float delta);

void init_weapon_sprite();

}   // Animations
