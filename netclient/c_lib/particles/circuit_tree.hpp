#pragma once

#include <compat.h>
#ifdef DC_CLIENT
    #include <compat_gl.h>
#endif

#include <t_map/t_map.h>
#include <t_map/t_properties.h>

#ifdef DC_CLIENT
    void circuit_tree_generate(int type, int seed);
    void circuit_tree_draw();
#endif
