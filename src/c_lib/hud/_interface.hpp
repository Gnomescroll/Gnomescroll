#pragma once

#include <hud/error.hpp>

namespace Hud
{

extern class HUD* hud;

void init();
void teardown();

//hud.cpp
void add_predicted_block_damage(int block_dmg);
void set_predicted_block_damage(int block_dmg);

void set_error_status(GSError err);
void set_hud_fps_display(float fps_val);
void init_hud_draw_settings();
void update_hud_draw_settings();
void draw();
void draw_error_status();

//harvest_bar.hpp
void start_harvest_bar();
void end_harvest_bar();
bool tick_harvest_bar();
void draw_harvest_bar(int x, int y);

}   // Hud
