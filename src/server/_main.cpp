#pragma once

#include <common/time/physics_timer.hpp>

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
    // t_gen::start_cave_generator();
    t_gen::excavate(); // corpusc copied cave_generator.hpp to excavator.hpp, with the idea to heavily modify it, without losing the original
    t_gen::populate_ore();
    t_gen::generate_ruins();
    t_gen::add_terrain_features();

    map_gen::rough_floor(XMAX,YMAX,0,3, t_map::get_cube_id("bedrock"));    
}


void init(int argc, char* argv[])
{
    //for (int i=0; i<argc; i++) 
        //printf("argument %d: %s\n", i, argv[i]);

    init_c_lib(argc, argv);

    srand(Options::seed);

    bool fast_map = false;
    bool corpusc = false;
    #if GS_SERIALIZER
    if (!serializer::load_data())
    {
        if (strcmp(Options::map, "fast") == 0)
            fast_map = true;
        else
        {
            default_map_gen();
            t_gen::populate_crystals();
            t_map::environment_process_startup();
        }
    }
    #else
    if (strcmp(Options::map, "fast") == 0)
        fast_map = true;
    else
    if (strcmp(Options::map, "corpusc") == 0)
        corpusc = true;
    else
    {   // load map from options if given; else load default map file; else do a map gen
        if ((Options::map[0] == '\0' || !serializer::load_map(Options::map))
        && !serializer::load_default_map())
            default_map_gen();
    }
    #endif

    if (corpusc)
    {
        map_gen::floor(XMAX,YMAX,0, 1, t_map::get_cube_id("bedrock"));
        t_gen::set_region(0,0,1, XMAX,YMAX,ZMAX/2, t_map::get_cube_id("regolith") );
        t_gen::excavate();
        //t_gen::generate_ruins();
        t_gen::add_terrain_features();
    }
    if (fast_map)
    {
        map_gen::floor(XMAX,YMAX,0, 1, t_map::get_cube_id("bedrock"));
        map_gen::floor(XMAX,YMAX,1, 9, t_map::get_cube_id("regolith"));
        map_gen::floor(XMAX,YMAX,20,1, t_map::get_cube_id("regolith"));
    }
                //t_gen::populate_crystals();

    srand((unsigned int)time(NULL));
    
    int address[4];
    address_from_string(Options::ip_address, address);
    NetServer::init_server(address[0],address[1],address[2],address[3], Options::port);

    #if !PRODUCTION
    //Objects::stress_test();
    #endif

    ServerState::init_base();

    ServerState:: main_inited = true;
    printf("Game started\n");
}

void tick()
{
    static int counter = 0;

    t_map::t_map_send_map_chunks();  //every tick

    if (counter % 15 == 0) 
    {
        Agents::agent_list->update_map_manager_positions();
        t_map::t_map_manager_update();
        //t_map::t_map_sort_map_chunk_ques();
    }

    Toolbelt::update_toolbelt_items();
    Toolbelt::tick();

    Agents::agent_list->update_models(); // sets skeleton
    
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
    if (counter % 6  == 0)
    {
        ItemContainer::check_agents_in_container_range();
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

    counter++;
}
 
int run()
{
    while (!ServerState::signal_exit)
    {
        int tc = 0;
        if (_GET_TICK())
        {
            tick();
            tc++;
        }

        if (tc > 0)
            NetServer::flush_to_net();
        if (tc > 1)
            printf("Warning:: %d ticks this frame", tc);
            
        NetServer::dispatch_network_events();
        if (Options::auth)
            NetServer::check_client_authorizations();
        
        if (serializer::should_save_map)
        {
            serializer::save_map();
            serializer::should_save_map = false;
            serializer::save_mechs();

            // TODO -- move, testing only
            if (Options::serializer)
            {
                serializer::save_map_palette_file();
                serializer::save_containers();
            }
        }

        serializer::update();

        gs_millisleep(1);
    }

    if (serializer::should_save_map)
    {
        serializer::save_map();
        serializer::should_save_map = false;
    }
        
    return 0;
}

}
