
//#ifndef c_lib_h
//#define c_lib_h

    #include "compat.h"
    #ifdef DC_CLIENT
        #include "compat_gl.h"
        GLint get_particle_sheet_id();
        //extern GLint particle_sheet_id;
    #endif

    int init_c_lib();

#define DC_SERVER

//#endif
