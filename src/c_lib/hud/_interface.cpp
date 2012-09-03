#include "_interface.hpp"

#include <hud/harvest_bar.hpp>

namespace Hud
{

	class HarvestBar* harvest_bar;
	class 		HUD* hud;

	void init()
	{
    	hud = new HUD();
    	hud->init();

		harvest_bar = new HarvestBar;
	}

	void teardown()
	{
		delete harvest_bar;
	}


	void start_harvest_bar(int max)
	{
		harvest_bar->start(max);
	}

	void end_harvest_bar()
	{
		harvest_bar->end();

	}

	bool tick_harvest_bar()
	{
		return harvest_bar->tick();
	}


}
