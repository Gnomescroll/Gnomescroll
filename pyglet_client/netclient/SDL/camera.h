#define NO_SDL_GLEXT
#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include <math.h>

#ifndef camera_h
#define camera_h

struct Camera {
float fov;
float x_size,y_size;
float ratio;
float z_near, z_far;

float x,y,z;
float xl, yl, zl;
float xu, yu, zu;
float x_angle, y_angle;
};

int _world_projection(struct Camera* camera);
int _hud_projection(struct Camera* camera);

int _set_camera(struct Camera* c);
struct Camera* _get_camera();

void set_model_view_matrix(float *a);

#endif
