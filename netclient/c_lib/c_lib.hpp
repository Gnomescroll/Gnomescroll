#pragma once

 #ifdef __cplusplus
 extern "C" {
 #endif
 
    #include "compat.h"
    #include "compat_gl.h"

    int init_c_lib();
    GLint get_particle_sheet_id();

 #ifdef __cplusplus
 }
 #endif

#define DC_CLIENT
