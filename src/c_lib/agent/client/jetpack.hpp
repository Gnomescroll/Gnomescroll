#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <sound/sound.hpp>

//const int JETPACK_FUEL_MAX = 1300;
const int JETPACK_FUEL_MAX = 500;
const int JETPACK_DRAIN_RATE = 3;
const int JETPACK_REFILL_RATE = 2;
const int JETPACK_MIN_TO_ACTIVATE = 8; // minimum fuel
const int JETPACK_LOOP_SOUND_BUILDUP = 30;
const int JETPACK_LOOP_SOUND_COOLDOWN = 20;

class JetPack
{
    private:
        // sound
        int loop_sound_id;
        int ticks_til_loop;
        int ticks_til_end;
        float prev_gain;
        
        bool prev_thrusting; // was previously thrusting
        bool underfueled_thrusting; // we are attempting to thrust but underfueld 

    public:
        int fuel;

    JetPack() :
        loop_sound_id(-1), ticks_til_loop(0), ticks_til_end(0),
        prev_gain(0.0f), prev_thrusting(false), underfueled_thrusting(false),
        fuel(JETPACK_FUEL_MAX)
    {}

    int update(bool thrusting)
    {
        float gain = 0.0f;

        if (thrusting)
        {   // we're starting a new thrust
            if (fuel >= JETPACK_MIN_TO_ACTIVATE)
            {
                if (!this->prev_thrusting)
                    this->ticks_til_loop = JETPACK_LOOP_SOUND_BUILDUP;

                // calculate the linear ramp up gain
                gain = 1.0f - ((float)this->ticks_til_loop/(float)JETPACK_LOOP_SOUND_BUILDUP);
                // in case we are starting a new thrust while the old one is cooling down,
                // keep the gain up until we catch up (otherwise the sound pops from the sudden gain switch)
                if (gain < this->prev_gain)
                    gain = this->prev_gain;

                // countdown the initial thrust
                this->ticks_til_loop--;
                this->ticks_til_loop = GS_MAX(0, this->ticks_til_loop);

                // burn fuel
                this->fuel -= JETPACK_DRAIN_RATE;
                this->fuel = GS_MAX(0, this->fuel);
                this->prev_thrusting = true;
            }
            else
            {   // we're trying to thrust but below the fuel threshold
                this->underfueled_thrusting = true;
                thrusting = false;
            }
        }

        if (!thrusting)
        {   // we've ended the thrust
            if (this->prev_thrusting)
                this->ticks_til_end = JETPACK_LOOP_SOUND_COOLDOWN;
            this->prev_thrusting = false;

            // calculate the linear ramp down gain
            gain = (float)this->ticks_til_end/(float)JETPACK_LOOP_SOUND_COOLDOWN;

            // in case we are ending a thrust while the old one is ramping up,
            // keep the gain down until we catch up (otherwise the sound pops from the sudden gain switch)
            if (gain > this->prev_gain)
                gain = this->prev_gain;

            // countdown to exit
            this->ticks_til_end--;
            this->ticks_til_end = GS_MAX(0, this->ticks_til_end);

            // regen fuel
            this->fuel += JETPACK_REFILL_RATE;
            this->fuel = GS_MIN(JETPACK_FUEL_MAX, this->fuel);
        }

        // check if still underfueled
        if (this->fuel >= JETPACK_MIN_TO_ACTIVATE)
            this->underfueled_thrusting = false;

        // if we're underfueld, move the gain to a low idle amount
        if (this->underfueled_thrusting)
        {
            gain = this->prev_gain - 0.10f;
            gain = GS_MAX(gain, 0.20f);
        }

        // Start sound, applying current gain
        if (gain > 0.0f && this->loop_sound_id < 0)
        {   // turn on
            this->loop_sound_id = Sound::play_2d_sound("jetpack_loop", gain, 1.0f);
        }

        // update sound properties, or stop it
        if (this->loop_sound_id >= 0)
        {
            if (gain > 0.0f)
            {   // apply gain
                Sound::set_gain_multiplier(this->loop_sound_id, gain);
            }
            else
            {   // turn off
                Sound::stop_sound(this->loop_sound_id);
                this->loop_sound_id = -1;
            }
        }

        // cache the gain
        this->prev_gain = gain;

        return thrusting;
    }
};
