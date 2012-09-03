
#pragma once


namespace Hud
{

	extern class HUD* hud;

	void init();

	void teardown();

	//hud
	void add_predicted_block_damage(int block_dmg);
	void set_predicted_block_damage(int block_dmg);

	void set_hud_fps_display(float fps_val);
	void init_hud_draw_settings();
	void update_hud_draw_settings();
	void draw_hud();

	//harvest bar
	void start_harvest_bar();
	void end_harvest_bar();
	bool tick_harvest_bar();
}
