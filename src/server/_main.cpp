#pragma once

#include <common/time/physics_timer.hpp>
#include <map_gen/map_generator.hpp>
#include <map_gen/recipes.hpp>

#include <t_gen/_interface.hpp>
#include <t_map/_interface.hpp>

#include <net_lib/server.hpp>

namespace Main
{


void default_map_gen()
{
    t_gen::noise_map_generate_map();
    
    t_map::map_post_processing(); //regolith stuff
    t_gen::generate_rock_layer();
    t_gen::start_cave_generator();
    t_gen::populate_ore();
    t_gen::generate_ruins();
    t_gen::add_terrain_features();

    map_gen::rough_floor(512,512,0,3, t_map::dat_get_cube_id("bedrock"));    
}


void init(int argc, char* argv[])
{
    //for (int i=0; i<argc; i++) 
        //printf("argument %d: %s\n", i, argv[i]);

    init_c_lib(argc, argv);

    srand(Options::seed);

    bool fast_map = false;
    #if GS_SERIALIZER
    if (!serializer::load_data())
    {
        if (strcmp(Options::map, "fast") == 0)
            fast_map = true;
        else
            default_map_gen();
    }
    #else
    if (Options::map[0] == '\0')
    {   // use map gen
        // load map file by default in development mode; decreases startup time
        if (!serializer::load_default_map())
            default_map_gen();
    }
    else if (!strcmp(Options::map, "fast"))
    {
        fast_map = true;
    }
    else
    {   // use map file
        serializer::load_map(Options::map);
    }   
    #endif

    if (fast_map)
    {
        map_gen::floor(512,512,0,1, t_map::dat_get_cube_id("bedrock"));
        map_gen::floor(512,512,1,9, t_map::dat_get_cube_id("regolith"));

        //map_gen::floor(512,512, 20,1, t_map::dat_get_cube_id("regolith"));
    
        //t_gen::generate_ruins();
        //t_gen::add_terrain_features();
    }
    else
    {
        // do this after map gen / loading until crystals are serialized
        t_gen::populate_crystals();
        t_map::environment_process_startup();
    }
    
    srand((unsigned int)time(NULL));
    
    int address[4];
    address_from_string(Options::ip_address, address);
    NetServer::init_server(address[0],address[1],address[2],address[3], Options::port);

    #if !PRODUCTION
    //Item::test_item_list_capacity();
    //ItemContainer::test_container_list_capacity();
    //Objects::stress_test();
    #endif

    ServerState::init_base();

   ServerState:: main_inited = true;
    printf("Game started\n");
}

void tick()
{
    static int counter = 0; counter ++;

    t_map::t_map_send_map_chunks();  //every tick

    if (counter % 15 == 0) 
    {
        ServerState::agent_list->update_map_manager_positions();
        t_map::t_map_manager_update();
        //t_map::t_map_sort_map_chunk_ques();
    }

    Toolbelt::update_toolbelt_items();
    Toolbelt::tick();

    ServerState::agent_list->update_models(); // sets skeleton
    
    Particle::grenade_list->tick();
    ItemParticle::tick();

    Objects::tick();
    Objects::harvest();
    Objects::update();

    Objects::spawn_mobs();

    ServerState::move_base();

    Components::rate_limit_component_list->call(); // advance rate limiter ticks
    Components::motion_targeting_component_list->call(); // update target lock ticks
    Components::agent_targeting_component_list->call(); // update target lock ticks

    if (counter % 10 == 0) ItemParticle::check_item_pickups();
    if (counter % 6  == 0) ItemContainer::check_agents_in_container_range();
    if (counter % 6  == 0)
    {
        Components::healer_component_list->call();
        ServerState::check_agents_at_base();
    }

    //ServerState::spawn_items(2);
    ServerState::spawn_monsters(OBJECT_MONSTER_BOMB, 50);
    ServerState::spawn_monsters(OBJECT_MONSTER_SPAWNER, 6);

    ItemContainer::update_smelters();
    Item::tick();

    t_map::environment_process_tick(); //refresh regolith etc...

    Auth::update(); // do it here because i need constant timer
}
 
int run()
{
    //int tick = 0;
    int tc;

    while (!ServerState::signal_exit)
    {
        tc = 0;
        while (1)
        {
            int ti = _GET_TICK();
            if (ti == 0 || tc > 1) break;

            tick();

            tc++;
            break;
        }

        if (tc > 0)
        {
            NetServer::flush_to_net();
        }

        if (tc > 1)
        {
            printf("Warning:: %i ticks this frame", tc);
        }
        NetServer::dispatch_network_events();
        if (Options::auth)
            NetServer::check_client_authorizations();
        
    #if GS_SERIALIZER
        if (serializer::should_save_map)
        {
            serializer::save_map();
            serializer::should_save_map = false;
            // TODO -- move, testing only
            serializer::save_containers();
        }

        serializer::update();
    #endif

        #ifdef __GNUC__
        usleep(1000);
        #endif
    
        #ifdef __MSVC__
        Sleep(1);
        #endif
    }

    if (serializer::should_save_map)
    {
        serializer::save_map();
        serializer::should_save_map = false;
    }
    
    close_c_lib();
    return 0;
}

}
