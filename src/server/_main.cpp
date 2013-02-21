#pragma once

#include <common/time/physics_timer.hpp>
#include <t_gen/_interface.hpp>
#include <t_map/_interface.hpp>
#include <net_lib/server.hpp>
#include <common/dat/attributes.hpp>

namespace Main
{

void default_map_gen()
{
    t_gen::noise_map_generate_map();

    t_map::map_post_processing();  // regolith stuff
    t_gen::generate_rock_layer();
    t_gen::start_cave_generator();
    t_gen::populate_ore();
    t_gen::populate_crystals();
    t_gen::generate_ruins();
    t_gen::add_terrain_features();  // gorges, trees, shrooms
    t_gen::place_native_plants();

    map_gen::rough_floor(t_map::map_dim.x,t_map::map_dim.y,0,3, t_map::get_cube_type("bedrock"));
}

void init_world()
{
    using t_map::map_dim;
    srand(Options::seed);

    bool new_map = false;
    bool valgrind_map = false;
    bool corpusc_map = false;
    bool art_map = false;
    bool explosive_map = false;
    bool iceflame_map = false;

    if (strcmp(Options::map, "valgrind") == 0)
        valgrind_map = true;
    else
    if (strcmp(Options::map, "new") == 0)
        new_map = true;
    else
    if (strcmp(Options::map, "art") == 0)
        art_map = true;
    else
    if (strcmp(Options::map, "corpusc") == 0)
        corpusc_map = true;
    else
    if (strcmp(Options::map, "explosive") == 0)
    {
        valgrind_map = true;
        explosive_map = true;
    }
    else
    if (strcmp(Options::map, "iceflame") == 0)
        iceflame_map = true;
    else
    if (strcmp(Options::map, "") == 0)
    {
        if (serializer::load_data())
        {
            if (Options::serializer)
            {   // only resave the data if we're in serializer mode
                bool saved = serializer::save_data();    // re-save immediately after loading, so that palette changes are up to date
                GS_ASSERT_ABORT(saved);
                serializer::wait_for_save_complete();
            }
        }
        else
            new_map = true;
    }
    else
    {
        printf("Unrecognized map option: %s\n", Options::map);
        exit(0);
    }

    if (new_map)
    {
        printf("Creating new map\n");
        serializer::begin_new_world_version();
        default_map_gen();
        t_map::environment_process_startup();
        if (Options::serializer)
        {
            bool saved = serializer::save_data();
            GS_ASSERT_ABORT(saved);
            serializer::wait_for_save_complete();
        }
    }

    if (corpusc_map)
    {
        int height = 27;
        map_gen::floor(map_dim.x, map_dim.y, 0, 1, t_map::get_cube_type("bedrock"));
        t_gen::set_region(0,0,1, map_dim.x, map_dim.y, height, t_map::get_cube_type("regolith"));
        //t_gen::excavate();
        t_gen::add_terrain_features();  // this needs like about 27 heighth to the ground or *CRASH*
        t_gen::generate_ruins();
        t_gen::make_art_gallery(height);
    }

    if (art_map)
    {
        int floor_h = 10; // height
        map_gen::floor(map_dim.x, map_dim.y, 0, 1, t_map::get_cube_type("bedrock"));
        t_gen::set_region(0,0,1, map_dim.x, map_dim.y, floor_h, t_map::get_cube_type("regolith"));
        t_gen::make_art_gallery(floor_h);
    }

    if (valgrind_map)
    {
        map_gen::floor(map_dim.x, map_dim.y, 0, 1, t_map::get_cube_type("bedrock"));
        map_gen::floor(map_dim.x, map_dim.y, 1, 9, t_map::get_cube_type("regolith"));
        map_gen::floor(map_dim.x, map_dim.y, 20, map_dim.z - 23, t_map::get_cube_type("regolith"));
        map_gen::floor(map_dim.x, map_dim.y, map_dim.z - 1, 1, t_map::get_cube_type("regolith"));
    }

    if (explosive_map)
    {
        static const CubeType plasmagen = t_map::get_cube_type("plasmagen");
        GS_ASSERT(t_map::isValidCube(plasmagen));
        static const size_t n_explosives = 10000;
        for (size_t i=0; i<n_explosives; i++)
        {
            int x = randrange(0, map_dim.x - 1);
            int y = randrange(0, map_dim.y - 1);
            int z = randrange(0, map_dim.z - 1);
            t_map::set_fast(x,y,z, plasmagen);
        }
    }

    if (iceflame_map)
    {
        default_map_gen();
        t_map::environment_process_startup();
        t_gen::generate_city();
    }

    srand((unsigned int)time(NULL));
}

void init(int argc, char* argv[])
{
    init_c_lib(argc, argv);

    init_world();

    int address[4] = {0};
    address_from_string(Options::ip_address, address);
    NetServer::init_server(address[0],address[1],address[2],address[3], Options::port);

    ServerState::init_base();

    ServerState:: main_inited = true;
    printf("Game started\n");
}

void tick()
{
    static int counter = 0;

    //map lighting update (non-essential, can be free time stuff)
    t_map::_lighting_rolling_update();
    t_map::_skylight_update_core(); //update sun lighting
    t_map::_envlight_update_core(); //update env lighting

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

    Entities::tick();
    Entities::harvest();
    Entities::update();

    Entities::spawn_mobs();

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

    if (counter % 7  == 0)
    {
        t_plant::tick();
    }

    //every half hour, respawn plants if food is low
    if (counter % (30*1800) == 0)
    {
        //t_plant::tick();
        int count = 0;
        count += t_mech::count_mech(t_mech::get_mech_type_dat("acadia_flower_stage_0"));
        count += t_mech::count_mech(t_mech::get_mech_type_dat("acadia_flower_stage_1"));
        count += t_mech::count_mech(t_mech::get_mech_type_dat("acadia_flower_stage_2"));

        if (count < 256 + 128)
        {
            printf("plants low: respawning native plants\n");
            t_gen::place_native_plants();
        }


        //printf("count= %d \n", count);
    }


    // Meteors
    const int meteor_fall_rate = ONE_HOUR * 24;
    const int meteor_shower_rate = ONE_HOUR * 24;
    #if PRODUCTION
    static int next_meteor_fall = 0;
    static int next_meteor_shower = 0;
    #else
    static int next_meteor_fall = ONE_MINUTE * 10;
    static int next_meteor_shower = ONE_MINUTE * 10;
    #endif

    if (counter >= next_meteor_fall)
    {
        t_gen::meteor_fall();
        next_meteor_fall += randrange(meteor_fall_rate/2, meteor_fall_rate);
    }
    if (counter >= next_meteor_shower)
    {
        t_gen::meteor_shower();
        next_meteor_shower += randrange(meteor_shower_rate/2, meteor_shower_rate);
    }

    //ServerState::spawn_items(2);
    ServerState::spawn_monsters(OBJECT_MONSTER_BOMB, 50);
    ServerState::spawn_monsters(OBJECT_MONSTER_SPAWNER, 6);

    ItemContainer::update_smelters();
    Item::tick();

    if (counter % 5 == 0) // 6 times a second
        t_mech::tick(); //t_mech growth timers
    if (counter % 30 == 0)
        t_mech::floating_removal_tick();

    t_map::environment_process_tick(); //refresh regolith etc...

    Auth::update(); // do it here because i need constant timer

    Agents::agent_list->tick_hunger();
    Agents::apply_agent_modifiers();

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
            Attributes::send_changes();
            NetServer::flush_to_net();
        }

        if (tc > 1)
            printf("Warning:: %d ticks this frame", tc);

        NetServer::dispatch_network_events();
        if (Options::auth)
            NetServer::check_client_authorizations();

        if (serializer::should_save_world)
        {
            bool saved = serializer::save_data();
            GS_ASSERT(saved);
            serializer::should_save_world = false;
        }

        serializer::update();

        // update the world save, else sleep
        if (serializer::update_save_state(2) == WORLD_SAVE_IDLE)
            gs_millisleep(1);

        if (ServerState::reload_settings)
        {
            LUA::load_options();
            ServerState::reload_settings = false;
        }
    }

    if (serializer::should_save_world)
    {
        bool saved = serializer::save_data();
        GS_ASSERT(saved);
        serializer::wait_for_save_complete();
        serializer::should_save_world = false;
    }

    return 0;
}

}   // Main
