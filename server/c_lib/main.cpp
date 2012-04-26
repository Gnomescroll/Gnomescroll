#pragma once

#include <common/time/physics_timer.hpp>
#include <map_gen/map_generator.hpp>
#include <map_gen/recipes.hpp>

namespace Main
{

//implementation

void init()
{
    init_c_lib();

    // generate floor map
    //_floor(512,512,0,32,1);

    MapGen::init();
    MapRecipes::simple_map();
    MapGen::teardown();
    srand(time(NULL));

    NetServer::init_server(127,0,0,1, Options::port);
    ServerState::start_game();

}

int run()
{
    //int tick = 0;
    int tc;
    while (1)
    {
        tc = 0;
        while(1)
        {
            int ti = _GET_TICK();
            if(ti == 0 || tc > 1) break;
            
            ServerState::server_tick();

            tc++;
            break;
        }

        if(tc > 0)
        {
            NetServer::flush_to_net();
        }

        if(tc > 1)
        {
            printf("Warning:: %i ticks this frame", tc);
        }
        NetServer::dispatch_network_events();

        usleep(1000);
    }
    close_c_lib();
    return 0;
}

}
