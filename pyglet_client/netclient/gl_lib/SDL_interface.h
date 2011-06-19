#ifndef __PYX_HAVE__SDL
#define __PYX_HAVE__SDL
#ifdef __cplusplus
#define __PYX_EXTERN_C extern "C"
#else
#define __PYX_EXTERN_C extern
#endif

/* "gl_lib/SDL_interface.pyx":11
 * 
 * ## Camera.c ##
 * cdef public struct Camera:             # <<<<<<<<<<<<<<
 *     float fov, x_size, y_size, z_near, z_far
 *     float x,y,z, x_angle,y_angle
 */

struct Camera {
  float fov;
  float x_size;
  float y_size;
  float z_near;
  float z_far;
  float x;
  float y;
  float z;
  float x_angle;
  float y_angle;
};

#ifndef __PYX_HAVE_API__SDL

#endif

PyMODINIT_FUNC initSDL(void);

#endif
