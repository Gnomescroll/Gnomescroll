#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif


const int JETPACK_FUEL_MAX = 1300;
const int JETPACK_DRAIN_RATE = 3;
const int JETPACK_REFILL_RATE = 2;



class JetPack
{
    public:
		int fuel;

	JetPack() {  
		fuel = JETPACK_FUEL_MAX;
	};



    int update(int thrusting) {
		if (!fuel) {
			thrusting = 0;
		}

		if (thrusting)
		{
			fuel -= JETPACK_DRAIN_RATE;
			if (fuel < 0) 
				fuel = 0;
		}
		else
		{
			fuel += JETPACK_REFILL_RATE;
			while (fuel >= JETPACK_FUEL_MAX)
			{
				fuel--;
			}
		}

		return thrusting;
	}
};