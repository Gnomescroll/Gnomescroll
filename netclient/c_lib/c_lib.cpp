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

/* objects */
#include <c_lib/particles/object_lib.cpp>
/* Weapons */
#include <c_lib/weapons/weapons.cpp>

/* Network */
#include <net_lib/net_lib.h>

/* Agents */
#include <c_lib/agent/agent_include.h>

#ifdef DC_CLIENT
    /* animations */
    #include <c_lib/animations/animations.cpp>
#endif

/* map */
#include <c_lib/t_map/t_map.cpp>
#include <c_lib/t_map/t_properties.c>
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

    /* input */
    #include <c_lib/input/input.cpp>

#endif

int init_c_lib() {
    printf("init c_lib\n");

    srand(time(NULL));   // seed the RNG

    #ifdef DC_CLIENT
        init_video();
        init_input();
        init_particle_functions();
        init_cameras();

        init_image_loader();
        init_particles();
        init_cube_selector();
        init_inventory();
    #endif
    
    return 0;
}
