#ifndef DC_CLIENT
    #define DC_CLIENT
#endif

#include <c_lib.hpp>

/* kill the compiler */
#ifdef DC_SERVER 
    sasfsafgdg4423+3
#endif

/* kill the compiler */
#ifndef DC_CLIENT
 asdfsfs;fgf;
#endif

/* objects */
#include <c_lib/objects/object_lib.cpp>

/* animations */
#include <c_lib/animations/animations.cpp>

/* map */
#include <c_lib/t_map/t_map.c>
#include <c_lib/t_map/t_properties.c>
#ifdef DC_CLIENT
    #include <c_lib/t_map/t_viz.c>
    #include <c_lib/t_map/t_vbo.c>
#endif

/* input */
#ifdef DC_CLIENT
    #include <c_lib/input/input.cpp>
#endif

/* camera */
/*
#ifdef DC_CLIENT
#include <c_lib/camera/camera.c>
#endif
*/

/* Vectors, Ray Tracers, Physics */
#include <c_lib/physics/vector.c>
#include <c_lib/ray_trace/ray_trace.c>
#include <c_lib/physics/common.c>

/* Agents */
#include <c_lib/agent/agent_include.h>

/* HUD */
#ifdef DC_CLIENT
    #include <c_lib/hud/cube_select.cpp>
    #include <c_lib/hud/inventory.cpp>
    #include <c_lib/texture_loader.c>
    #include <c_lib/hud/hud_texture_loader.cpp>
#endif

/* SDL */
#ifdef DC_CLIENT
    #include <c_lib/SDL/camera.c>
    #include <c_lib/SDL/draw_functions.c>
    #include <c_lib/SDL/particle_functions.c>
    #include <c_lib/SDL/SDL_functions.c>
    #include <c_lib/SDL/SDL_text.c>
    #include <c_lib/SDL/texture_loader.c>
    #include <c_lib/SDL/block_selector.c>
#endif

/* Network */
#include <net_lib/net_lib.h>

int init_c_lib() {
    printf("init c_lib\n");
    srand(time(NULL));   // seed the RNG

    #ifdef DC_CLIENT
        init_texture_loader();
        init_hud_texture_loader();
    #endif
return 0;
}
