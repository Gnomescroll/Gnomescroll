#include <SDL.h>
#include <SDL_opengl.h>

#include <math.h>

struct Camera {
float fov;
float x_size;
float y_size;
float z_near;
float z_far;

float x,y,z;
float x_angle;
float y_angle;
};

int _world_projection(struct Camera* camera);
int _hud_projection(struct Camera* camera);
