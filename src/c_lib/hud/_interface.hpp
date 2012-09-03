
#pragma once


namespace Hud
{
	void init();

	void teardown();

	//harvest bar
	void start_harvest_bar();
	void end_harvest_bar();
	bool tick_harvest_bar();
}
