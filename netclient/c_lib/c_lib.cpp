#ifndef DC_CLIENT
    #define DC_CLIENT
#endif

/* kill the compiler */
#ifdef DC_SERVER 
    sasfsafgdg4423+3
#endif

#include <c_lib.hpp>


/* Network */
#include <net_lib/net_lib.h>


/* Graphics */
#include "compat_gl.h"
#include "defines.h"

//utility
#include <c_lib/common/enum_types.hpp>
#include <c_lib/time/physics_timer.c>

/* Vectors, Ray Tracers, Physics */
#include <c_lib/physics/vector.cpp>
#include <c_lib/physics/vec3.hpp>
#include <c_lib/physics/vec4.hpp>
#include <c_lib/physics/color_matrix.cpp>
#include <c_lib/ray_trace/ray_trace.c>
#include <c_lib/ray_trace/hitscan.cpp>
#include <c_lib/physics/common.c>

/* Voxel Models */
#include <c_lib/voxel/voxel_volume.cpp>
#include <c_lib/voxel/voxel_render.cpp>
#include <c_lib/voxel/voxel_hitscan.cpp>
#include <c_lib/voxel/voxel_body.cpp>
#include <c_lib/voxel/object_vox.cpp>

/* particles */
#include <c_lib/particles/particle_lib.cpp>

/* Items */
#include <c_lib/items/items.cpp>

/* Game Stuff */
#include <c_lib/game/game.cpp>
#include <c_lib/game/teams.cpp>
#include <c_lib/game/ctf.cpp>
#include <c_lib/game/packets.cpp>

/* Agents */
#include <c_lib/agent/agent_include.h>

/* Monsters */
#include <c_lib/monsters/packets.cpp>
#include <c_lib/monsters/monsters.cpp>

/* Weapons */
#include <c_lib/weapons/weapons.cpp>
#include <c_lib/weapons/packets.cpp>

#ifdef DC_CLIENT
    /* animations */
    #include <c_lib/animations/animations.cpp>
    #include <c_lib/animations/hitscan.cpp>
#endif

/* map */
#include <c_lib/t_map/t_map.cpp>
#include <c_lib/t_map/t_properties.c>
#include <c_lib/t_map/t_serialize.c>

#ifdef DC_CLIENT
    #include <c_lib/t_map/t_viz.c>
    #include <c_lib/t_map/t_vbo.c>

    /* camera */
    #include <c_lib/camera/camera.cpp>
    #include <c_lib/camera/fulstrum_test.cpp>

    /* skybox */
    #include <c_lib/skybox/skybox.cpp>

    /* SDL */
    #include <c_lib/SDL/shader_loader.cpp>
    #include <c_lib/SDL/texture_loader.c>
    #include <c_lib/SDL/draw_functions.c>
    #include <c_lib/SDL/particle_functions.c>
    #include <c_lib/SDL/SDL_functions.c>

#ifdef linux
    #include <c_lib/SDL/IMG_savepng.c>
#endif

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

#include <c_lib/export.cpp>

/* client side map gen / utils */
#include <c_lib/map_gen/hopcroft-karp.cpp>
#include <c_lib/map_gen/dragon.cpp>

#include <c_lib/state/client_state.cpp>
#include <c_lib/state/packet_init.cpp>

//#include <c_lib/SDL/pngfuncs.c>
//#include <c_lib/SDL/v.cpp>

//page size
#include <unistd.h>
//(size_t) sysconf(_SC_PAGESIZE);

int init_c_lib() {
    //printf("System page size= %li \n", sysconf(_SC_PAGESIZE) );
    printf("init c_lib\n");

    srand(time(NULL));   // seed the RNG

    init_network();
    init_net_client();
    
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
        //vn::init();

        Animations::init_hitscan();
        
        Sound::init();

        ClientState::init();
    #endif
    
    return 0;
}


void close_c_lib() {
    printf("close c_lib() \n");
    shutdown_net_client();
    #ifdef DC_CLIENT
        Sound::close();
        //close_SDL();  //would be called twice, already scheduled for at exit
    #endif
}
