#pragma once

extern "C" {
    #include "compat.h"
    #include "compat_gl.h"
}

extern "C" {
    int init_c_lib();
    GLint get_particle_sheet_id();
}

#define DC_CLIENT
