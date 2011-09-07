#ifndef c_lib_h
#define c_lib_h

#include "compat.h"
#ifdef DC_CLIENT
#include "compat_gl.h"
#endif

int init_c_lib();
#ifdef DC_CLIENT
GLint get_particle_sheet_id();
#endif
//extern GLint particle_sheet_id;

#define DC_SERVER

#endif
