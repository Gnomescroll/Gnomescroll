#include "sound.h"

void load_projectile();
void end_projectile();

int play_projectile_sound(float x, float y, float z, float vx, float vy, float vz);
int update_projectile_sound(int pid, float x, float y, float z, float vx, float vy, float vz);
