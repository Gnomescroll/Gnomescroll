
#pragma once


namespace Hud
{

class HarvestBar
{
	public:

	int max;	//total to fil
	int level;

	HarvestBar() {};

	void start(int _max)
	{
		max = max;
	}

	bool tick()
	{
		max++;

		if(max >= level)
			return true;

		return false;
	}

	void end()
	{

	}

	void draw(int x, int y)
	{


	}


};

}
