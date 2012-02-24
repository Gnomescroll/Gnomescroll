#include "frame_rate_limiter.hpp"

#include <c_lib/time/physics_timer.h>

int frame_left()
{
    static int last_time =  _GET_MS_TIME();

    int current_time = _GET_MS_TIME();


    if( current_time - last_time < 15)
    {
        //nanosleep( 1000*(current_time - last_time) );
        usleep(1000*(15-(current_time - last_time) ));
        //printf("slept for: %i ms \n",15-(current_time - last_time ));
        //wait(current_time - last_time);
    }
    else
    {
        //printf("overshot frame: %i ms since last frame \n", current_time - last_time);
    }


    last_time = current_time;

    static int counter=0;
    static int counter_t =_GET_MS_TIME();
    counter++;

    if( current_time - counter_t > 1000 ) 
    {
        //printf("%i frames per second \n", counter);
        counter = 0;
        counter_t = current_time;
    }
    return 0;

}