#include "projectile_sounds.h"

FMOD_SOUND* projectile_sound;
FMOD_CHANNEL* projectile_channels[MAX_SOUNDS];


void load_projectile() {
    projectile_sound = load_3d_sound("./media/sound/wav/warp2.wav", 1000.0f);
}

void end_projectile() {
    release_sound(projectile_sound);
}


void play_set_channel(int id, float x, float y, float z, float vx, float vy, float vz) {
    const FMOD_VECTOR pos = create_vector(x,y,z);
    const FMOD_VECTOR vec = create_vector(vx,vy,vz);
    FMOD_CHANNEL* ch = play_3d_sound(projectile_sound, pos, vec);
    projectile_channels[id] = ch;
    //FMOD_Channel_SetCallback(ch, cb);
}

int play_projectile_sound(float x, float y, float z, float vx, float vy, float vz) {
    int i;
    int set=0;
    FMOD_BOOL* is_playing;
    FMOD_RESULT r;
    FMOD_CHANNEL* pch;

    for(i=0; i<MAX_SOUNDS; i++) {
        pch = projectile_channels[i];
        if (pch == NULL) {
            play_set_channel(i, x,y,z, vx,vy,vz);
            set = 1;
        } else {
            r = FMOD_Channel_IsPlaying(pch, is_playing);
            ERRCHECK(r);
            if (!is_playing) {
                play_set_channel(i, x,y,z, vx,vy,vz);
                set = 1;
            }
        }
    }
    if (!set) {
        i = -1;
    }
    const FMOD_VECTOR pos = create_vector(x,y,z);
    const FMOD_VECTOR vel = create_vector(vx, vy, vz);

    return i;
}

int update_projectile_sound(int pid, float x, float y, float z, float vx, float vy, float vz) {
    FMOD_CHANNEL* pch = projectile_channels[pid];
    int i;
    if (pch != NULL) {
        const FMOD_VECTOR pos = create_vector(x,y,z);
        const FMOD_VECTOR vel = create_vector(vx, vy, vz);
        i = update_channel(pch, pos, vel);
        return i;
    }
    return 1;
}



FMOD_RESULT F_CALLBACK channel_end_callback(FMOD_CHANNEL *channel, FMOD_CHANNEL_CALLBACKTYPE type, void *commanddata1, void *commanddata2)
{
    
    return FMOD_OK; 
}

