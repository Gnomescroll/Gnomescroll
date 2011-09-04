#include "projectile_sounds.h"

FMOD_SOUND* projectile_sound;
FMOD_CHANNEL* projectile_channels[MAX_SOUNDS];


void load_projectile() {
    projectile_sound = load_3d_sound("./media/sound/wav/warp2.wav", 1000.0f);
}

void end_projectile() {
    release_sound(projectile_sound);
}


int play_projectile_sound(float x, float y, float z, float vx, float vy, float vz) {
    const FMOD_VECTOR pos = create_vector(x,y,z);
    const FMOD_VECTOR vec = create_vector(vx,vy,vz);
    FMOD_CHANNEL* ch = play_3d_sound(projectile_sound, pos, vec);
    int i;
    int set=0;
    FMOD_BOOL* is_playing;
    FMOD_RESULT r;

    for(i=0; i<MAX_SOUNDS; i++) {
        FMOD_CHANNEL* pch = projectile_channels[i];
        if (pch == NULL) {
            projectile_channels[i] = pch;
            set = 1;
        } else {
            r = FMOD_Channel_IsPlaying(pch, is_playing);
            ERRCHECK(r);
            if (!is_playing) {
                projectile_channels[i] = pch;
                set = 1;
            }
        }
    }
    if (!set) {
        i = -1;
    }
    return i;
}

int update_projectile_sound(int pid, float x, float y, float z, float vx, float vy, float vz) {
    FMOD_CHANNEL* pch = projectile_channels[pid];
    const FMOD_VECTOR pos = create_vector(x,y,z);
    const FMOD_VECTOR vel = create_vector(vx, vy, vz);
    int i;
    if (pch != NULL) {
        i = update_channel(pch, pos, vel);
        return i;
    }
    return 1;
}

