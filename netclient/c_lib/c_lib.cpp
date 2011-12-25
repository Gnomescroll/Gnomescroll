#ifndef DC_CLIENT
    #define DC_CLIENT
#endif

/* kill the compiler */
#ifdef DC_SERVER 
    sasfsafgdg4423+3
#endif

#include <c_lib.hpp>

#include <defines.h>

/* Vectors, Ray Tracers, Physics */
#include <c_lib/physics/vector.cpp>
#include <c_lib/ray_trace/ray_trace.c>
#include <c_lib/ray_trace/hitscan.cpp>
#include <c_lib/physics/common.c>

/* Voxel Models */
#include <c_lib/voxel/voxel_volume.cpp>

/* particles */
#include <c_lib/particles/particle_lib.cpp>
/* Weapons */
#include <c_lib/weapons/weapons.cpp>
/* Items */
#include <c_lib/items/items.cpp>

/* Game Stuff */
#include <c_lib/game/game.cpp>
#include <c_lib/game/teams.cpp>
#include <c_lib/game/ctf.cpp>
#include <c_lib/game/packets.cpp>

/* Agents */
#include <c_lib/agent/agent_include.h>

#ifdef DC_CLIENT
    /* animations */
    #include <c_lib/animations/animations.cpp>
#endif

/* map */
#include <c_lib/t_map/t_map.cpp>
#include <c_lib/t_map/t_properties.c>

/* Network */
#include <net_lib/net_lib.h>

#ifdef DC_CLIENT
    #include <c_lib/t_map/t_viz.c>
    #include <c_lib/t_map/t_vbo.c>

    /* camera */
    #include <c_lib/camera/camera.cpp>

    /* skybox */
    #include <c_lib/skybox/skybox.cpp>

    /* SDL */
    #include <c_lib/SDL/texture_loader.c>
    #include <c_lib/SDL/draw_functions.c>
    #include <c_lib/SDL/particle_functions.c>
    #include <c_lib/SDL/SDL_functions.c>

    /* HUD */
    #include <c_lib/hud/cube_selector.cpp>
    #include <c_lib/hud/inventory.cpp>
    #include <c_lib/hud/text.c>
    #include <c_lib/hud/map.cpp>
    #include <c_lib/hud/equipment.cpp>
    #include <c_lib/hud/compass.cpp>

    /* input */
    #include <c_lib/input/input.cpp>

    /* sound */
    #include <c_lib/sound/sound.cpp>

#endif

#include <c_lib/state/client_state.cpp>
#include <c_lib/state/packet_init.cpp>

//page size
#include <unistd.h>
//(size_t) sysconf(_SC_PAGESIZE);

int init_c_lib() {
    //printf("System page size= %li \n", sysconf(_SC_PAGESIZE) );
    printf("init c_lib\n");

    srand(time(NULL));   // seed the RNG

    #ifdef DC_CLIENT
        init_video();
        init_image_loader();
        init_input();
        init_particle_functions();
        init_cameras();

        init_particles();
        init_cube_selector();
        init_inventory();
        HudMap::init();
        HudEquipment::init();
        Compass::init();

        Sound::init();
    #endif

    ClientState::ctf.init();
    
    return 0;
}


void close_c_lib() {
    printf("close c_lib() \n");
    #ifdef DC_CLIENT
        Sound::close();
        //close_SDL();  //would be called twice, already scheduled for at exit
    #endif
}
