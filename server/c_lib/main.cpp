#include "main.hpp"

#include <c_lib/common/time/physics_timer.hpp>

namespace Main
{

//implementation

void init()
{
    init_c_lib();

    // generate map
    _floor(512,512,0,32,1);

    NetServer::init_server(127,0,0,1, Options::port);
    ServerState::start_game();

}

/*
    tick = 0

    while True:

        while True: #physics loop
            tc = GET_TICK()
            if tc == 0 or sl_c > 1: #net out
                sl_c+=1
                break

            init_c_lib.tick_server_state()

            tick+=1

            NetServerFlushToNet()

        if sl_c > 0:
            while GET_TICK() != 0:
                sl_c+=1
        #if sl_c > 1:
            #print "Physics: %i ticks this frame" % (sl_c)
        

        NetServerDispatchNetworkEvents()
        

        time.sleep(0.001)

    init_c_lib.close()
*/

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
