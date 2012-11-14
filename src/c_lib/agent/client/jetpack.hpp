#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include "sound/sound.hpp"

#include <sound/packets.hpp>
#include <sound/triggers.hpp>


const int JETPACK_FUEL_MAX = 1300;
const int JETPACK_DRAIN_RATE = 3;
const int JETPACK_REFILL_RATE = 2;
const int JETPACK_MIN_TO_ACTIVATE = 8; // minimum fuel
const int JETPACK_LOOP_DELAY = 30;



class JetPack {
	int jetpack_loop_sound_id;
	int ticks_til_loop; 
    bool prev_thrusting; // was previously thrusting
public:
	int fuel;

	JetPack() {
		jetpack_loop_sound_id = -1;
		ticks_til_loop = -1; 
		prev_thrusting = false;
		fuel = JETPACK_FUEL_MAX;
	};



	int update(int thrusting) {
		ticks_til_loop--;
		if (ticks_til_loop == 0) {
			ticks_til_loop = -1; 
			////Sound::play_2d_sound("jetpack_loop");
			//GS_ASSERT(this->jetpack_loop_sound_id < 0);
			//if (this->jetpack_loop_sound_id < 0) 
			//	this->jetpack_loop_sound_id = Sound::play_2d_sound("jetpack_loop");
		}

		if (thrusting) {
			if (fuel >= JETPACK_MIN_TO_ACTIVATE) {
				if (!prev_thrusting) {
					Sound::play_2d_sound("jetpack_on");
					ticks_til_loop = JETPACK_LOOP_DELAY;
				}

				fuel -= JETPACK_DRAIN_RATE;
				if (fuel < 0) 
					fuel = 0;

				prev_thrusting = true;
			} else {
				prev_thrusting = false;
				thrusting = false;
			}
		} else {
			if (prev_thrusting) { // && this->jetpack_loop_sound_id >= 0) {
				//Sound::jetpack_loop(false, this->jetpack_loop_sound_id);
				////Sound::play_2d_sound("jetpack_loop", false, this->jetpack_loop_sound_id);
				//this->jetpack_loop_sound_id = -1;

				///////////////Sound::play_2d_sound("jetpack_off");
			}
			prev_thrusting = false;

			fuel += JETPACK_REFILL_RATE;
			while (fuel >= JETPACK_FUEL_MAX) {
				fuel--;
			}
		}

		return thrusting;
	}
};